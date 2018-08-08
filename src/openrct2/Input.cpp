/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Input.h"

#include "Context.h"

uint8_t gInputPlaceObjectModifier;

widget_ref gHoverWidget;
widget_ref gPressedWidget;

TOOL_IDX gCurrentToolId;
widget_ref gCurrentToolWidget;

// TODO: Move to openrct2-ui and make static again
INPUT_STATE gInputState;
uint8_t gInputFlags;
uint16_t gTooltipsNotShownTicks;
widget_ref gDragWidget;
int32_t gCurrentScrollIndex;
int32_t gCurrentScrollArea;

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
    if (on)
    {
        gInputFlags |= flag;
    }
    else
    {
        gInputFlags &= ~flag;
    }
}

bool input_test_flag(INPUT_FLAGS flag)
{
    return gInputFlags & flag;
}

void input_reset_flags()
{
    gInputFlags = 0;
}

void input_set_state(INPUT_STATE state)
{
    gInputState = state;
}

INPUT_STATE input_get_state()
{
    return gInputState;
}

void reset_tooltip_not_shown()
{
    gTooltipsNotShownTicks = 0;
}

void input_reset_place_obj_modifier()
{
    gInputPlaceObjectModifier = PLACE_OBJECT_MODIFIER_NONE;
}
