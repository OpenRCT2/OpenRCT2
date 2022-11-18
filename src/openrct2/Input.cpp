/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Input.h"

#include "Context.h"

InputState _inputState;
uint8_t _inputFlags;
uint8_t gInputPlaceObjectModifier;

widget_ref gHoverWidget;
widget_ref gPressedWidget;

uint16_t _tooltipNotShownTicks;

Tool gCurrentToolId;
widget_ref gCurrentToolWidget;

/**
 *
 *  rct2: 0x006E3B43
 */
void title_handle_keyboard_input()
{
    ContextInputHandleKeyboard(true);
}

/**
 *
 *  rct2: 0x006E3B43
 */
void game_handle_keyboard_input()
{
    ContextInputHandleKeyboard(false);
}

void input_set_flag(INPUT_FLAGS flag, bool on)
{
    if (on)
    {
        _inputFlags |= flag;
    }
    else
    {
        _inputFlags &= ~flag;
    }
}

bool input_test_flag(INPUT_FLAGS flag)
{
    return _inputFlags & flag;
}

void input_reset_flags()
{
    _inputFlags = 0;
}

void input_set_state(InputState state)
{
    _inputState = state;
}

InputState input_get_state()
{
    return _inputState;
}

void reset_tooltip_not_shown()
{
    _tooltipNotShownTicks = 0;
}

void input_reset_place_obj_modifier()
{
    gInputPlaceObjectModifier = PLACE_OBJECT_MODIFIER_NONE;
}
