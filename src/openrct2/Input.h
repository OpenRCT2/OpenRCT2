/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

struct widget_ref
{
    rct_windowclass window_classification;
    rct_windownumber window_number;
    rct_widgetindex widget_index;
};

extern uint8_t gInputPlaceObjectModifier;
extern widget_ref gHoverWidget;
extern widget_ref gPressedWidget;

extern Tool gCurrentToolId;
extern widget_ref gCurrentToolWidget;

static ScreenCoordsXY gInputDragLast;
static ScreenCoordsXY gInputScrollXY;

// TODO: Move to openrct2-ui and make static again
extern InputState _inputState;
extern uint8_t _inputFlags;

void InputWindowPositionBegin(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);

void title_handle_keyboard_input();
void GameHandleInput();
void game_handle_keyboard_input();
void GameHandleEdgeScroll();
int32_t GetNextKey();

void StoreMouseInput(MouseState state, const ScreenCoordsXY& screenCoords);

void input_set_flag(INPUT_FLAGS flag, bool on);
bool input_test_flag(INPUT_FLAGS flag);
void input_reset_flags();

bool InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER modifier);

void input_set_state(InputState state);
InputState input_get_state();

void input_reset_place_obj_modifier();

void InputScrollViewport(const ScreenCoordsXY& screenCoords);
