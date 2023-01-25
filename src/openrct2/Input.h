/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "interface/Window.h"

enum INPUT_FLAGS
{
    INPUT_FLAG_WIDGET_PRESSED = (1 << 0),

    // The dropdown can stay open if the mouse is released, set on flag Dropdown::Flag::StayOpen
    INPUT_FLAG_DROPDOWN_STAY_OPEN = (1 << 1),

    // The mouse has been released and the dropdown is still open
    // INPUT_FLAG_DROPDOWN_STAY_OPEN is already set if this happens
    INPUT_FLAG_DROPDOWN_MOUSE_UP = (1 << 2),

    INPUT_FLAG_TOOL_ACTIVE = (1 << 3),

    // Left click on a viewport
    INPUT_FLAG_4 = (1 << 4),

    INPUT_FLAG_5 = (1 << 5),

    // Some of the map tools (clear, footpath, scenery)
    // never read as far as I know.
    INPUT_FLAG_6 = (1 << 6),

    INPUT_FLAG_VIEWPORT_SCROLLING = (1 << 7)
};

enum class MouseState : uint32_t
{
    Released,
    LeftPress,
    LeftRelease,
    RightPress,
    RightRelease
};

enum class InputState
{
    Reset,
    Normal,
    WidgetPressed,
    PositioningWindow,
    ViewportRight,
    DropdownActive,
    ViewportLeft,
    ScrollLeft,
    Resizing,
    ScrollRight
};

enum PLACE_OBJECT_MODIFIER
{
    PLACE_OBJECT_MODIFIER_NONE = 0,
    PLACE_OBJECT_MODIFIER_SHIFT_Z = (1 << 0),
    PLACE_OBJECT_MODIFIER_COPY_Z = (1 << 1),
};

struct WidgetRef
{
    WindowClass window_classification;
    rct_windownumber window_number;
    WidgetIndex widget_index;
};

extern uint8_t gInputPlaceObjectModifier;

extern ScreenCoordsXY gInputDragLast;

extern WidgetRef gHoverWidget;
extern WidgetRef gPressedWidget;

extern uint16_t gTooltipTimeout;
extern WidgetRef gTooltipWidget;
extern ScreenCoordsXY gTooltipCursor;

extern Tool gCurrentToolId;
extern WidgetRef gCurrentToolWidget;

// TODO: Move to openrct2-ui and make static again
extern InputState _inputState;
extern uint8_t _inputFlags;
extern uint16_t _tooltipNotShownTicks;

void InputWindowPositionBegin(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);

void TitleHandleKeyboardInput();
void GameHandleInput();
void GameHandleKeyboardInput();
void GameHandleEdgeScroll();
int32_t GetNextKey();

void StoreMouseInput(MouseState state, const ScreenCoordsXY& screenCoords);

void InputSetFlag(INPUT_FLAGS flag, bool on);
bool InputTestFlag(INPUT_FLAGS flag);
void InputResetFlags();

bool InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER modifier);

void InputSetState(InputState state);
InputState InputGetState();

void ResetTooltipNotShown();

void InputResetPlaceObjModifier();

void InputScrollViewport(const ScreenCoordsXY& screenCoords);
