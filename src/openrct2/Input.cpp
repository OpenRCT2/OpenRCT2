#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "Context.h"
#include "Input.h"

INPUT_STATE _inputState;
uint8 _inputFlags;
uint8 gInputPlaceObjectModifier;

widget_ref gHoverWidget;
widget_ref gPressedWidget;

uint16 _tooltipNotShownTicks;

TOOL_IDX gCurrentToolId;
widget_ref gCurrentToolWidget;

/**
 *
 *  rct2: 0x006E3B43
 */
void title_handle_keyboard_input()
{
    context_input_handle_keyboard(true);
}

/**
 *
 *  rct2: 0x006E3B43
 */
void game_handle_keyboard_input()
{
    context_input_handle_keyboard(false);
}

void input_set_flag(INPUT_FLAGS flag, bool on)
{
    if (on) {
        _inputFlags |= flag;
    } else {
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

void input_set_state(INPUT_STATE state)
{
    _inputState = state;
}

INPUT_STATE input_get_state()
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
