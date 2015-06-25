/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "../addresses.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../world/map.h"
#include "../game.h"
#include "../interface/themes.h"
#include "../interface/title_sequences.h"
#include "../title.h"
#include "dropdown.h"

typedef struct {
	uint8 command;
	rct_string_id nameStringId;
	rct_string_id descStringId;
} TITLE_COMMAND_ORDER;

TITLE_COMMAND_ORDER window_title_command_editor_orders[] = {
	{ TITLE_SCRIPT_LOAD,		5413, 5431 },
	{ TITLE_SCRIPT_LOCATION,	5417, 5427 },
	{ TITLE_SCRIPT_ROTATE,		5419, 5428 },
	{ TITLE_SCRIPT_ZOOM,		5421, 5429 },
	{ TITLE_SCRIPT_SPEED,		5442, 5444 },
	{ TITLE_SCRIPT_WAIT,		5423, 5430 },
	{ TITLE_SCRIPT_RESTART,		5425, STR_NONE },
	{ TITLE_SCRIPT_END,			5426, STR_NONE },
};

#define NUM_COMMANDS 8

enum WINDOW_WATER_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_COMMAND,
	WIDX_COMMAND_DROPDOWN,
	WIDX_TEXTBOX_FULL,
	WIDX_TEXTBOX_X,
	WIDX_TEXTBOX_Y,
	WIDX_INPUT,
	WIDX_INPUT_DROPDOWN,
	WIDX_OKAY,
	WIDX_CANCEL
};

#define WW 200
#define WH 120
#define BY 32
#define BY2 70
#define WS 16
#define WHA ((WW-WS*2)/2)

static bool _window_title_command_editor_insert;
static int _window_title_command_editor_index;
static char textbox1Buffer[50];
static char textbox2Buffer[50];
static title_command command = { 6, 0, 0 };

static rct_widget window_title_command_editor_widgets[] = {
	{ WWT_FRAME,				1,	0,			WW-1,		0,		WH-1,	-1,							STR_NONE },							// panel / background
	{ WWT_CAPTION,				1,	1,			WW-2,		1,		14,		5434,						STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,				1,	WW-13,		WW-3,		2,		13,		824,						STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_DROPDOWN,				1,	WS,			WW-WS-1,	BY,		BY+11,	STR_NONE,					STR_NONE }, // Command dropdown
	{ WWT_DROPDOWN_BUTTON,		1,	WW-WS-12,	WW-WS-2,	BY+1,	BY+10,	876,						STR_NONE },
	{ WWT_TEXT_BOX,				1,	WS,			WW-WS-1,	BY2,	BY2+11,	(uint32)textbox1Buffer,		STR_NONE }, // full textbox

	{ WWT_TEXT_BOX,				1,	WS,			WS+WHA-4,	BY2,	BY2+11,	(uint32)textbox1Buffer,		STR_NONE }, // x textbox
	{ WWT_TEXT_BOX,				1,	WS+WHA+3,	WW-WS-1,	BY2,	BY2+11,	(uint32)textbox2Buffer,		STR_NONE }, // y textbox

	{ WWT_DROPDOWN,				1,	16,			WW-17,		BY2,	BY2+11,	STR_NONE,					STR_NONE }, // Save dropdown
	{ WWT_DROPDOWN_BUTTON,		1,	WW-28,		WW-18,		BY2+1,	BY2+10,	876,						STR_NONE },
	
	{ WWT_DROPDOWN_BUTTON,		1,	10,			80,			WH-21,	WH-10,	STR_OK,						STR_NONE }, // OKAY
	{ WWT_DROPDOWN_BUTTON,		1,	WW-80,		WW-10,		WH-21,	WH-10,	STR_CANCEL,					STR_NONE }, // Cancel

	{ WIDGETS_END },
};

static void window_title_command_editor_emptysub() { }
static void window_title_command_editor_close();
static void window_title_command_editor_mouseup();
static void window_title_command_editor_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_title_command_editor_dropdown();
static void window_title_command_editor_update();
static void window_title_command_editor_invalidate();
static void window_title_command_editor_paint();
static void window_title_command_editor_textinput();
static void window_title_command_editor_inputsize(rct_window *w);
static int get_command_info_index(int index);
static TITLE_COMMAND_ORDER get_command_info(int index);
static rct_xy16 get_location();

static void* window_title_command_editor_events[] = {
	window_title_command_editor_close,
	window_title_command_editor_mouseup,
	window_title_command_editor_emptysub,
	window_title_command_editor_mousedown,
	window_title_command_editor_dropdown,
	window_title_command_editor_emptysub,
	window_title_command_editor_update,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_textinput,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_emptysub,
	window_title_command_editor_invalidate,
	window_title_command_editor_paint,
	window_title_command_editor_emptysub
};

static int get_command_info_index(int index)
{
	for (int i = 0; i < NUM_COMMANDS; i++) {
		if (window_title_command_editor_orders[i].command == index)
			return i;
	}
	return 0;
}

static TITLE_COMMAND_ORDER get_command_info(int index)
{
	for (int i = 0; i < NUM_COMMANDS; i++) {
		if (window_title_command_editor_orders[i].command == index)
			return window_title_command_editor_orders[i];
	}
	return window_title_command_editor_orders[0];
}

static rct_xy16 get_location()
{
	rct_xy16 mapCoord = { 0 };
	rct_window *w = window_get_main();
	if (w != NULL) {
		int interactionType;
		rct_map_element *mapElement;

		get_map_coordinates_from_pos(w->viewport->view_width / 2, w->viewport->view_height / 2, VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);
		mapCoord.x -= 16;
		mapCoord.x /= 32;
		mapCoord.y -= 16;
		mapCoord.y /= 32;
		mapCoord.x++;
		mapCoord.y++;
	}
	return mapCoord;
}

void window_title_command_editor_open(int index, bool insert)
{
	rct_window* window;

	// Check if window is already open
	if (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != NULL)
		return;

	window = window_create_centred(
		WW, 
		WH,
		(uint32*)window_title_command_editor_events, 
		WC_TITLE_COMMAND_EDITOR,
		WF_STICK_TO_FRONT
	);
	window->widgets = window_title_command_editor_widgets;
	window->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_COMMAND) |
		(1 << WIDX_COMMAND_DROPDOWN) |
		(1 << WIDX_TEXTBOX_FULL) |
		(1 << WIDX_TEXTBOX_X) |
		(1 << WIDX_TEXTBOX_Y) |
		(1 << WIDX_INPUT) |
		(1 << WIDX_INPUT_DROPDOWN) |
		(1 << WIDX_OKAY) |
		(1 << WIDX_CANCEL);
	window_init_scroll_widgets(window);

	_window_title_command_editor_index = index;
	_window_title_command_editor_insert = insert;
	if (!insert) {
		command = gConfigTitleSequences.presets[gCurrentTitleSequence].commands[index];
	}

	switch (command.command) {
	case TITLE_SCRIPT_LOAD:
		if (command.saveIndex >= gConfigTitleSequences.presets[gCurrentTitleSequence].num_commands)
			command.saveIndex = 0xFF;
		break;
	case TITLE_SCRIPT_LOCATION:
		_itoa(command.x, textbox1Buffer, 10);
		_itoa(command.y, textbox2Buffer, 10);
		break;
	case TITLE_SCRIPT_ROTATE:
	case TITLE_SCRIPT_ZOOM:
	case TITLE_SCRIPT_WAIT:
		_itoa(command.rotations, textbox1Buffer, 10);
		break;
	}
}

static void window_title_command_editor_close()
{
	rct_window *w;

	window_get_register(w);
}

static void window_title_command_editor_mouseup()
{
	rct_window *w, *title_editor_w;
	short widgetIndex;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
	case WIDX_CANCEL:
		window_close(w);
		break;
	case WIDX_TEXTBOX_FULL:
		window_start_textbox(w, widgetIndex, 1170, (uint32)textbox1Buffer, 4);
		break;
	case WIDX_TEXTBOX_X:
		window_start_textbox(w, widgetIndex, 1170, (uint32)textbox1Buffer, 4);
		break;
	case WIDX_TEXTBOX_Y:
		window_start_textbox(w, widgetIndex, 1170, (uint32)textbox2Buffer, 4);
		break;
	case WIDX_OKAY:
		if (_window_title_command_editor_insert) {
			title_sequence_insert_command(gCurrentTitleSequence, _window_title_command_editor_index, command);
		}
		else {
			gConfigTitleSequences.presets[gCurrentTitleSequence].commands[_window_title_command_editor_index] = command;
			title_sequence_save_preset_script(gCurrentTitleSequence);
		}
		title_editor_w = window_find_by_class(WC_TITLE_EDITOR);
		if (title_editor_w != NULL)
			title_editor_w->selected_list_item = _window_title_command_editor_index;
		window_close(w);
		break;
	}
}


static void window_title_command_editor_mousedown(int widgetIndex, rct_window* w, rct_widget* widget)
{
	int num_items, i;

	widget--;
	switch (widgetIndex) {
	case WIDX_COMMAND_DROPDOWN:
		num_items = NUM_COMMANDS;
		for (i = 0; i < num_items; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = window_title_command_editor_orders[i].nameStringId;
		}
		
		window_dropdown_show_text_custom_width(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			num_items,
			widget->right - widget->left - 3
			);

		gDropdownItemsChecked = 1 << get_command_info_index(command.command);
		break;
	case WIDX_INPUT_DROPDOWN:
		if (command.command == TITLE_SCRIPT_SPEED) {
			num_items = 4;
			for (i = 0; i < num_items; i++) {
				gDropdownItemsFormat[i] = 1142;
				gDropdownItemsArgs[i] = 5142 + i;
			}

			window_dropdown_show_text_custom_width(
				w->x + widget->left,
				w->y + widget->top,
				widget->bottom - widget->top + 1,
				w->colours[1],
				DROPDOWN_FLAG_STAY_OPEN,
				num_items,
				widget->right - widget->left - 3
				);

			gDropdownItemsChecked = 1 << (command.speed - 1);
		}
		else if (command.command == TITLE_SCRIPT_LOAD) {
			num_items = gConfigTitleSequences.presets[gCurrentTitleSequence].num_saves;
			for (i = 0; i < num_items; i++) {
				gDropdownItemsFormat[i] = 2777;
				gDropdownItemsArgs[i] = (uint64)&gConfigTitleSequences.presets[gCurrentTitleSequence].saves[i];
			}

			window_dropdown_show_text_custom_width(
				w->x + widget->left,
				w->y + widget->top,
				widget->bottom - widget->top + 1,
				w->colours[1],
				DROPDOWN_FLAG_STAY_OPEN,
				num_items,
				widget->right - widget->left - 3
				);

			gDropdownItemsChecked = 1 << (command.saveIndex);
		}
		break;
	}
}

static void window_title_command_editor_dropdown()
{
	rct_window* w;
	short widgetIndex, dropdownIndex;
	rct_xy16 mapCoord;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);
	
	if (dropdownIndex == -1)
		return;

	switch (widgetIndex) {
	case WIDX_COMMAND_DROPDOWN:
		if (dropdownIndex == get_command_info_index(command.command))
			break;
		command.command = window_title_command_editor_orders[dropdownIndex].command;
		switch (command.command) {
		case TITLE_SCRIPT_LOCATION:
			mapCoord = get_location();
			command.x = (uint8)mapCoord.x;
			command.y = (uint8)mapCoord.y;
			_itoa(command.x, textbox1Buffer, 10);
			_itoa(command.y, textbox2Buffer, 10);
			break;
		case TITLE_SCRIPT_ROTATE:
			command.rotations = 1;
			_itoa(command.rotations, textbox1Buffer, 10);
			break;
		case TITLE_SCRIPT_ZOOM:
			command.zoom = 0;
			_itoa(command.zoom, textbox1Buffer, 10);
			break;
		case TITLE_SCRIPT_SPEED:
			command.speed = 1;
			break;
		case TITLE_SCRIPT_WAIT:
			command.seconds = 10;
			_itoa(command.seconds, textbox1Buffer, 10);
			break;
		case TITLE_SCRIPT_LOAD:
			command.saveIndex = 0;
			if (command.saveIndex >= gConfigTitleSequences.presets[gCurrentTitleSequence].num_commands)
				command.saveIndex = 0xFF;
			break;
		}
		window_invalidate(w);
		break;
	case WIDX_INPUT_DROPDOWN:
		if (command.command == TITLE_SCRIPT_SPEED) {
			if (dropdownIndex == command.speed - 1)
				break;
			command.speed = (uint8)(dropdownIndex + 1);
		}
		else if (command.command == TITLE_SCRIPT_LOAD) {
			if (dropdownIndex == command.saveIndex)
				break;
			command.saveIndex = (uint8)dropdownIndex;
		}
		window_invalidate(w);
		break;
	}
}

static void window_title_command_editor_textinput()
{
	uint8 result;
	short widgetIndex;
	rct_window *w;
	char *text, * end;
	int value;

	window_textinput_get_registers(w, widgetIndex, result, text);
	
	value = strtol(widgetIndex != WIDX_TEXTBOX_Y ? textbox1Buffer : textbox2Buffer, &end, 10);
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	switch (widgetIndex) {
	case WIDX_TEXTBOX_FULL:
		if (!result) {
			if (*end == '\0') {
				if (command.command == TITLE_SCRIPT_ROTATE || command.command == TITLE_SCRIPT_ZOOM) {
					if (value > 3) value = 3;
				}
				else if (command.command == TITLE_SCRIPT_WAIT) {
					if (value < 1) value = 1;
				}
				command.rotations = (uint8)value;
			}
			_itoa(command.rotations, textbox1Buffer, 10);
			window_invalidate(w);
		}
		else {
			strcpy(textbox1Buffer, text);
		}
		break;
	case WIDX_TEXTBOX_X:
		if (!result) {
			if (*end == '\0')
				command.x = (uint8)value;
			_itoa(command.x, textbox1Buffer, 10);
			window_invalidate(w);
		}
		else {
			strcpy(textbox1Buffer, text);
		}
		break;
	case WIDX_TEXTBOX_Y:
		if (!result) {
			if (*end == '\0')
				command.y = (uint8)value;
			_itoa(command.y, textbox2Buffer, 10);
			window_invalidate(w);
		}
		else {
			strcpy(textbox2Buffer, text);
		}
		break;
	}
}

static void window_title_command_editor_update(rct_window *w)
{
	if (gCurrentTextBox.window.classification == w->classification &&
		gCurrentTextBox.window.number == w->number) {
		window_update_textbox_caret();
		widget_invalidate(w, gCurrentTextBox.widget_index);
	}
}

static void window_title_command_editor_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update_by_class(w, WC_TITLE_EDITOR);

	window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].type = WWT_EMPTY;
	window_title_command_editor_widgets[WIDX_TEXTBOX_X].type = WWT_EMPTY;
	window_title_command_editor_widgets[WIDX_TEXTBOX_Y].type = WWT_EMPTY;
	window_title_command_editor_widgets[WIDX_INPUT].type = WWT_EMPTY;
	window_title_command_editor_widgets[WIDX_INPUT_DROPDOWN].type = WWT_EMPTY;
	switch (command.command) {
	case TITLE_SCRIPT_LOAD:
	case TITLE_SCRIPT_SPEED:
		window_title_command_editor_widgets[WIDX_INPUT].type = WWT_DROPDOWN;
		window_title_command_editor_widgets[WIDX_INPUT_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		break;
	case TITLE_SCRIPT_LOCATION:
		window_title_command_editor_widgets[WIDX_TEXTBOX_X].type = WWT_TEXT_BOX;
		window_title_command_editor_widgets[WIDX_TEXTBOX_Y].type = WWT_TEXT_BOX;
		break;
	case TITLE_SCRIPT_ROTATE:
	case TITLE_SCRIPT_ZOOM:
	case TITLE_SCRIPT_WAIT:
		window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].type = WWT_TEXT_BOX;
		break;
	}
}

static void window_title_command_editor_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	gfx_draw_string_left(dpi, 5432, NULL, w->colours[1], w->x + WS, w->y + BY - 14);
	gfx_draw_string_left(dpi, get_command_info(command.command).descStringId, NULL, w->colours[1], w->x + WS, w->y + BY2 - 14);

	gfx_draw_string_left_clipped(
		dpi,
		get_command_info(command.command).nameStringId,
		NULL,
		w->colours[1],
		w->x + w->widgets[WIDX_COMMAND].left + 1,
		w->y + w->widgets[WIDX_COMMAND].top,
		w->widgets[WIDX_COMMAND_DROPDOWN].left - w->widgets[WIDX_COMMAND].left - 4
		);

	if (command.command == TITLE_SCRIPT_SPEED) {
		gfx_draw_string_left_clipped(
			dpi,
			5142 + command.speed - 1,
			NULL,
			w->colours[1],
			w->x + w->widgets[WIDX_INPUT].left + 1,
			w->y + w->widgets[WIDX_INPUT].top,
			w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4
			);
	}
	else if (command.command == TITLE_SCRIPT_LOAD) {
		if (command.saveIndex == 0xFF) {
			gfx_draw_string_left_clipped(
				dpi,
				5437,
				NULL,
				w->colours[1],
				w->x + w->widgets[WIDX_INPUT].left + 1,
				w->y + w->widgets[WIDX_INPUT].top,
				w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4
				);
		}
		else {
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&gConfigTitleSequences.presets[gCurrentTitleSequence].saves[command.saveIndex];
			gfx_draw_string_left_clipped(
				dpi,
				1170,
				(void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS,
				w->colours[1],
				w->x + w->widgets[WIDX_INPUT].left + 1,
				w->y + w->widgets[WIDX_INPUT].top,
				w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4
				);
		}
	}
}
