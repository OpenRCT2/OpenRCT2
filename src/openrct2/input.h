#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#ifndef _INPUT_H_
#define _INPUT_H_

#include "interface/window.h"

typedef enum INPUT_FLAGS {
	INPUT_FLAG_WIDGET_PRESSED = (1 << 0),

	// The dropdown can stay open if the mouse is released, set on flag DROPDOWN_FLAG_STAY_OPEN
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
} INPUT_FLAGS;

enum MOUSE_STATE {
	MOUSE_STATE_RELEASED,
	MOUSE_STATE_LEFT_PRESS,
	MOUSE_STATE_LEFT_RELEASE,
	MOUSE_STATE_RIGHT_PRESS,
	MOUSE_STATE_RIGHT_RELEASE
};

typedef enum INPUT_STATE {
	INPUT_STATE_RESET,
	INPUT_STATE_NORMAL,
	INPUT_STATE_WIDGET_PRESSED,
	INPUT_STATE_POSITIONING_WINDOW,
	INPUT_STATE_VIEWPORT_RIGHT,
	INPUT_STATE_DROPDOWN_ACTIVE,
	INPUT_STATE_VIEWPORT_LEFT,
	INPUT_STATE_SCROLL_LEFT,
	INPUT_STATE_RESIZING,
	INPUT_STATE_SCROLL_RIGHT
} INPUT_STATE;

typedef enum PLACE_OBJECT_MODIFIER {
	PLACE_OBJECT_MODIFIER_NONE = 0,
	PLACE_OBJECT_MODIFIER_SHIFT_Z = (1 << 0),
	PLACE_OBJECT_MODIFIER_COPY_Z = (1 << 1),
} PLACE_OBJECT_MODIFIER;

typedef struct widget_ref {
	rct_windowclass window_classification;
	rct_windownumber window_number;
	rct_widgetindex widget_index;
} widget_ref;

extern uint8 gInputPlaceObjectModifier;

extern sint32 gInputDragLastX;
extern sint32 gInputDragLastY;

extern widget_ref gHoverWidget;
extern widget_ref gPressedWidget;

extern uint16 gTooltipTimeout;
extern widget_ref gTooltipWidget;
extern sint32 gTooltipCursorX;
extern sint32 gTooltipCursorY;

extern TOOL_IDX gCurrentToolId;
extern widget_ref gCurrentToolWidget;

void input_window_position_begin(rct_window *w, sint32 widgetIndex, sint32 x, sint32 y);

void title_handle_keyboard_input();
void game_handle_input();
void game_handle_keyboard_input();

void store_mouse_input(sint32 state, sint32 x, sint32 y);

void input_set_flag(INPUT_FLAGS flag, bool on);
bool input_test_flag(INPUT_FLAGS flag);
void input_reset_flags();

bool input_test_place_object_modifier(PLACE_OBJECT_MODIFIER modifier);

void input_set_state(INPUT_STATE state);
INPUT_STATE input_get_state();

void reset_tooltip_not_shown();

void input_reset_place_obj_modifier();

#endif
