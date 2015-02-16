/*****************************************************************************
* Copyright (c) 2014 Dániel Tar
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
#include "../audio/audio.h"
#include "../config.h"
#include "../editor.h"
#include "../scenario.h"
#include "../sprites.h"
#include "../localisation/localisation.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../platform/platform.h"
#include "../title.h"
#include "../util/util.h"
#include "error.h"

enum {
	WIDX_PREVIOUS_IMAGE,		// 1
	WIDX_PREVIOUS_STEP_BUTTON,	// 2
	WIDX_NEXT_IMAGE,			// 4
	WIDX_NEXT_STEP_BUTTON,		// 8
};

static rct_widget window_editor_bottom_toolbar_widgets[] = {
	{ WWT_IMGBTN, 0, 0, 199, 0, 33, 0xFFFFFFFF, 0xFFFF },			// 1		0x9A9958
	{ WWT_FLATBTN, 0, 2, 197, 2, 31, 0xFFFFFFFF, 0xFFFF },			// 2		0x9A9968
	{ WWT_IMGBTN, 0, 440, 639, 0, 33, 0xFFFFFFFF, 0xFFFF },			// 4		0x9A9978
	{ WWT_FLATBTN, 0, 442, 637, 2, 31, 0xFFFFFFFF, 0xFFFF },		// 8		0x9A9988
	{ WIDGETS_END },
};

static void window_editor_bottom_toolbar_emptysub() { }

static void window_editor_bottom_toolbar_mouseup();
static void window_editor_bottom_toolbar_invalidate();
static void window_editor_bottom_toolbar_paint();

static void window_editor_bottom_toolbar_jump_back_to_object_selection();
static void window_editor_bottom_toolbar_jump_back_to_landscape_editor();
static void window_editor_bottom_toolbar_jump_back_to_invention_list_set_up();
static void window_editor_bottom_toolbar_jump_back_to_options_selection();

static void window_editor_bottom_toolbar_jump_forward_from_object_selection();
static void window_editor_bottom_toolbar_jump_forward_to_invention_list_set_up();
static void window_editor_bottom_toolbar_jump_forward_to_options_selection();
static void window_editor_bottom_toolbar_jump_forward_to_objective_selection();
static void window_editor_bottom_toolbar_jump_forward_to_save_scenario();

static void* window_editor_bottom_toolbar_events[] = {
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_mouseup,	//0x0066f5ae,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_invalidate, //0x0066f1c9,
	window_editor_bottom_toolbar_paint, //0x0066f25c,
	window_editor_bottom_toolbar_emptysub
};

static EMPTY_ARGS_VOID_POINTER* previous_button_mouseup_events[] = {
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_jump_back_to_object_selection,
	window_editor_bottom_toolbar_jump_back_to_landscape_editor,
	window_editor_bottom_toolbar_jump_back_to_invention_list_set_up,
	window_editor_bottom_toolbar_jump_back_to_options_selection,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_jump_back_to_object_selection,
	window_editor_bottom_toolbar_emptysub
};

static EMPTY_ARGS_VOID_POINTER* next_button_mouseup_events[] = {
	window_editor_bottom_toolbar_jump_forward_from_object_selection, 
	window_editor_bottom_toolbar_jump_forward_to_invention_list_set_up,
	window_editor_bottom_toolbar_jump_forward_to_options_selection,
	window_editor_bottom_toolbar_jump_forward_to_objective_selection,
	window_editor_bottom_toolbar_jump_forward_to_save_scenario,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub,
	window_editor_bottom_toolbar_emptysub
};

/**
* Creates the main editor top toolbar window.
* rct2: 0x0066F052 (part of 0x0066EF38)
*/
void window_editor_bottom_toolbar_open()
{
	rct_window* window;

	window = window_create(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) - 32,
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16), 32,
		(uint32*)window_editor_bottom_toolbar_events,
		WC_BOTTOM_TOOLBAR, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_5);
	window->widgets = window_editor_bottom_toolbar_widgets;

	window->enabled_widgets |=
		(1 << WIDX_PREVIOUS_STEP_BUTTON) |
		(1 << WIDX_NEXT_STEP_BUTTON) |
		(1 << WIDX_PREVIOUS_IMAGE) |
		(1 << WIDX_NEXT_IMAGE);

	window_init_scroll_widgets(window);
	window->colours[0] = 150;
	window->colours[1] = 150;
	window->colours[2] = 141;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)) {
		window->colours[0] = 135;
		window->colours[1] = 135;
		window->colours[2] = 135;
	}
}

/**
*
*  rct2: 0x0066F619
*/
void window_editor_bottom_toolbar_jump_back_to_object_selection() {
	window_close_all();
	g_editor_step = EDITOR_STEP_OBJECT_SELECTION;
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066F62C
*/
void window_editor_bottom_toolbar_jump_back_to_landscape_editor() {
	window_close_all();
	RCT2_CALLPROC_EBPSAFE(0x006DFED0);
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	g_editor_step = EDITOR_STEP_LANDSCAPE_EDITOR;
	window_map_open();
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066F64E
*/
void window_editor_bottom_toolbar_jump_back_to_invention_list_set_up() {
	window_close_all();
	window_editor_inventions_list_open();
	g_editor_step = EDITOR_STEP_INVENTIONS_LIST_SET_UP;
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066F666
*/
void window_editor_bottom_toolbar_jump_back_to_scenario_options() {
	window_close_all();
	window_editor_scenario_options_open();
	g_editor_step = EDITOR_STEP_OPTIONS_SELECTION;
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066F64E
*/
void window_editor_bottom_toolbar_jump_back_to_options_selection() {
	window_close_all();
	window_editor_scenario_options_open();
	g_editor_step = EDITOR_STEP_OPTIONS_SELECTION;
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066F6B0
*/
void window_editor_bottom_toolbar_jump_forward_from_object_selection() {
	RCT2_CALLPROC_EBPSAFE(0x0066f6b0);
}

/**
*
*  rct2: 0x0066f758
*/
void window_editor_bottom_toolbar_jump_forward_to_invention_list_set_up() {
	uint32 flags = RCT2_CALLPROC_X(0x0066FEAC, 0, 0, 0, 0, 0, 0, 0);

	if (!(flags & 0x100)) {
		window_close_all();
		window_editor_inventions_list_open();
		g_editor_step = EDITOR_STEP_INVENTIONS_LIST_SET_UP;
	} else {
		window_error_open(STR_CANT_ADVANCE_TO_NEXT_EDITOR_STAGE, RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16));
	}

	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066f790
*/
void window_editor_bottom_toolbar_jump_forward_to_options_selection() {
	window_close_all();
	window_editor_scenario_options_open();
	g_editor_step = EDITOR_STEP_OPTIONS_SELECTION;
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066f7a8
*/
void window_editor_bottom_toolbar_jump_forward_to_objective_selection() {
	window_close_all();
	window_editor_objective_options_open();
	g_editor_step = EDITOR_STEP_OBJECTIVE_SELECTION;
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x00675181
 */
static int show_save_scenario_dialog(char *resultPath)
{
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	int result;
	char title[256];
	char filename[MAX_PATH];
	char filterName[256];


	format_string(title, STR_SAVE_SCENARIO, NULL);
	subsitute_path(filename, RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char), s6Info->name);
	strcat(filename, ".SC6");
	format_string(filterName, STR_RCT2_SCENARIO_FILE, NULL);

	pause_sounds();
	result = platform_open_common_file_dialog(0, title, filename, "*.SC6", filterName);
	unpause_sounds();

	if (result)
		strcpy(resultPath, filename);
	return result;
}

/**
 *
 *  rct2: 0x0066F7C0
 */
void window_editor_bottom_toolbar_jump_forward_to_save_scenario()
{
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;
	int parkFlagsBackup, success;
	char path[256];

	if (!scenario_prepare_for_save()) {
		window_error_open(STR_UNABLE_TO_SAVE_SCENARIO_FILE, RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id));
		gfx_invalidate_screen();
		return;
	}

	window_close_all();

	window_loadsave_open(LOADSAVETYPE_SAVE | LOADSAVETYPE_SCENARIO);
	return;

	if (!show_save_scenario_dialog(path)) {
		gfx_invalidate_screen();
		return;
	}

	// 
	s6Info->var_000 = 255;

	// Ensure path has .SC6 extension
	path_set_extension(path, ".SC6");

	// Save the scenario
	parkFlagsBackup = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32);
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) &= ~PARK_FLAGS_18;
	success = scenario_save(path, gConfigGeneral.save_plugin_data ? 3 : 2);
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) = parkFlagsBackup;

	if (success) {
		// RCT2_CALLPROC_EBPSAFE(0x0066DC83);
		title_load();
	} else {
		window_error_open(STR_SCENARIO_SAVE_FAILED, -1);
		s6Info->var_000 = 4;
	}
}

/**
*
*  rct2: 0x0066F5AE
*/
static void window_editor_bottom_toolbar_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	if (widgetIndex == WIDX_PREVIOUS_STEP_BUTTON) {
		if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) ||
			RCT2_GLOBAL(0x13573C8, uint16) == 0x2710 && !(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_18)) {
			previous_button_mouseup_events[g_editor_step]();
		}
	} else if (widgetIndex == WIDX_NEXT_STEP_BUTTON) {
		next_button_mouseup_events[g_editor_step]();
	}
}

void hide_previous_step_button() {
	window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WWT_EMPTY;
	window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].type = WWT_EMPTY;
}

void hide_next_step_button() {
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].type = WWT_EMPTY;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].type = WWT_EMPTY;
}

/**
*
*  rct2: 0x0066F1C9
*/
void window_editor_bottom_toolbar_invalidate() {
	rct_window* w;

	window_get_register(w);

	sint16 screenWidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left = screenWidth - 198;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right = screenWidth - 3;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].left = screenWidth - 200;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].right = screenWidth - 1;

	window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WWT_FLATBTN;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].type = WWT_FLATBTN;
	window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].type = WWT_IMGBTN;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].type = WWT_IMGBTN;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
		hide_previous_step_button();
		hide_next_step_button();
	} else {
		if (g_editor_step == EDITOR_STEP_OBJECT_SELECTION) {
			hide_previous_step_button();
		} else if (g_editor_step == EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			hide_next_step_button();
		} else if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)) {
			if (RCT2_GLOBAL(0x13573C8, uint16) != 0x2710 || RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_18) {
				hide_previous_step_button();
			}
		}
	}
}

/**
*
*  rct2: 0x0066F25C
*/
void window_editor_bottom_toolbar_paint() {
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	bool drawPreviousButton = false;
	bool drawNextButton = false;

	if (g_editor_step == EDITOR_STEP_OBJECT_SELECTION) {
		drawNextButton = true;
	} 
	else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) {
		drawPreviousButton = true;
	} 
	else if (RCT2_GLOBAL(0x13573C8, uint16) != 0x2710) {
		drawNextButton = true;
	}
	else if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_18) {
		drawNextButton = true;
	} 
	else {
		drawPreviousButton = true;
	}

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER)) {
		if (drawPreviousButton) {
			gfx_fill_rect(dpi,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + w->y,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].bottom + w->y, 0x2000033);
		}

		if ((drawPreviousButton || drawNextButton) && g_editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			gfx_fill_rect(dpi,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + w->y,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].bottom + w->y, 0x2000033);
		}
	}

	window_draw_widgets(w, dpi);

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER)) {
		

		if (drawPreviousButton) {
			gfx_fill_rect_inset(dpi,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + 1 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + 1 + w->y,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right - 1 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].bottom - 1 + w->y,
				w->colours[1], 0x30);
		}

		if ((drawPreviousButton || drawNextButton) && g_editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			gfx_fill_rect_inset(dpi,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left + 1 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + 1 + w->y,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right - 1 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].bottom - 1 + w->y,
				w->colours[1], 0x30);
		}

		short stateX =
			(window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right +
			window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left) / 2 + w->x;
		short stateY = w->height - 0x0C + w->y;
		gfx_draw_string_centred(dpi, STR_OBJECT_SELECTION_STEP + g_editor_step,
			stateX, stateY, (w->colours[2] & 0x7F) | 0x20, 0);

		if (drawPreviousButton) {
			gfx_draw_sprite(dpi, SPR_PREVIOUS,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + 6 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + 6 + w->y, 0);

			int textColour = w->colours[1] & 0x7F;

			if (RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, uint8) == WC_BOTTOM_TOOLBAR &&
				RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, uint8) == WIDX_PREVIOUS_STEP_BUTTON)
				textColour = 2;

			short textX = (window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + 30 +
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right) / 2 + w->x;
			short textY = window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + 6 + w->y;

			short stringId = STR_OBJECT_SELECTION_STEP + g_editor_step - 1;
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
				stringId = STR_OBJECT_SELECTION_STEP;

			gfx_draw_string_centred(dpi, STR_BACK_TO_PREVIOUS_STEP, textX, textY, textColour, 0);
			gfx_draw_string_centred(dpi, stringId, textX, textY + 10, textColour, 0);
		}

		if ((drawPreviousButton || drawNextButton) && g_editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			gfx_draw_sprite(dpi, SPR_NEXT,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right - 29 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + 6 + w->y, 0);

			int textColour = w->colours[1] & 0x7F;

			if (RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, uint8) == WC_BOTTOM_TOOLBAR &&
				RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, uint8) == WIDX_NEXT_STEP_BUTTON)
				textColour = 2;

			short textX = (window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left +
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right - 30) / 2 + w->x;
			short textY = window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + 6 + w->y;
			
			short stringId = STR_OBJECT_SELECTION_STEP + g_editor_step + 1;
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
				stringId = STR_ROLLERCOASTER_DESIGNER_STEP;

			gfx_draw_string_centred(dpi, STR_FORWARD_TO_NEXT_STEP, textX, textY, textColour, 0);
			gfx_draw_string_centred(dpi, stringId, textX, textY + 10, textColour, 0);

		}
	}
}