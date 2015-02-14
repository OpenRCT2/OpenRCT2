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
#include "../audio/audio.h"
#include "../config.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../openrct2.h"
#include "../sprites.h"
#include "../tutorial.h"

enum WINDOW_SAVE_PROMPT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_LABEL,
	WIDX_SAVE,
	WIDX_DONT_SAVE,
	WIDX_CANCEL
};

static rct_widget window_save_prompt_widgets[] = {
	{ WWT_FRAME,			0,	0,		259,	0,	49,	STR_NONE,					STR_NONE },					// panel / background
	{ WWT_CAPTION,			0,	1,		258,	1,	14,	0,							STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0,	247,	257,	2,	13,	STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_12,				0,	2,		257,	19,	30,	0,							STR_NONE },					// question/label
	{ WWT_DROPDOWN_BUTTON,	0,	8,		85,		35,	46,	STR_SAVE_PROMPT_SAVE,		STR_NONE },		// save
	{ WWT_DROPDOWN_BUTTON,	0,	91,		168,	35,	46,	STR_SAVE_PROMPT_DONT_SAVE,	STR_NONE },		// don't save
	{ WWT_DROPDOWN_BUTTON,	0,	174,	251,	35,	46,	STR_SAVE_PROMPT_CANCEL,		STR_NONE },		// cancel
	{ WIDGETS_END },
};

enum WINDOW_QUIT_PROMPT_WIDGET_IDX {
	WQIDX_BACKGROUND,
	WQIDX_TITLE,
	WQIDX_CLOSE,
	WQIDX_OK,
	WQIDX_CANCEL
};

static rct_widget window_quit_prompt_widgets[] = {
	{ WWT_FRAME,			0,	0,		176,	0,	33,	STR_NONE,					STR_NONE },					// panel / background
	{ WWT_CAPTION,			0,	1,		175,	1,	14,	STR_QUIT_GAME_PROMPT_TITLE,	STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0,	164,	174,	2,	13,	STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_DROPDOWN_BUTTON,	0,	8,		85,		19,	30,	STR_OK,						STR_NONE },		// ok
	{ WWT_DROPDOWN_BUTTON,	0,	91,		168,	19,	30,	STR_CANCEL,					STR_NONE },		// cancel
	{ WIDGETS_END },
};

static void window_save_prompt_emptysub() { }
static void window_save_prompt_close();
static void window_save_prompt_mouseup();
static void window_save_prompt_paint();

static void* window_save_prompt_events[] = {
	window_save_prompt_close,
	window_save_prompt_mouseup,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_emptysub,
	window_save_prompt_paint,
	window_save_prompt_emptysub
};

/**
 *
 *  rct2: 0x0066DCBE
 */
void window_save_prompt_open()
{
	int stringId, width, height;
	rct_window* window;
	unsigned short prompt_mode;
	rct_widget *widgets;
	uint64 enabled_widgets;

	prompt_mode = RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16);

	// do not show save prompt if we're in the title demo and click on load game
	if (prompt_mode != PM_QUIT && RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TITLE_DEMO) {
		game_load_or_quit_no_save_prompt();
		return;
	}

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_SAVE_PROMPT);
	if (window){
		window_close(window);
	}

	if (prompt_mode == PM_QUIT) {
		widgets = window_quit_prompt_widgets;
		enabled_widgets =
			(1 << WQIDX_CLOSE) |
			(1 << WQIDX_OK) |
			(1 << WQIDX_CANCEL);
		width = 177;
		height = 34;
	} else {
		widgets = window_save_prompt_widgets;
		enabled_widgets =
			(1 << WIDX_CLOSE) |
			(1 << WIDX_SAVE) |
			(1 << WIDX_DONT_SAVE) |
			(1 << WIDX_CANCEL);
		width = 260;
		height = 50;
	}

	window = window_create_centred(
		width,
		height,
		(uint32*)window_save_prompt_events,
		WC_SAVE_PROMPT,
		WF_TRANSPARENT | WF_STICK_TO_FRONT
	);

	window->widgets = widgets;
	window->enabled_widgets = enabled_widgets;
	window_init_scroll_widgets(window);
	window->colours[0] = 154;

	// Pause the game
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) |= 2;
	pause_sounds();
	window_invalidate_by_class(WC_TOP_TOOLBAR);

	stringId = prompt_mode + STR_LOAD_GAME_PROMPT_TITLE;
	if (stringId == STR_LOAD_GAME_PROMPT_TITLE && RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
		stringId = STR_LOAD_LANDSCAPE_PROMPT_TITLE;
	if (stringId == STR_QUIT_GAME_PROMPT_TITLE && RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
		stringId = STR_QUIT_SCENARIO_EDITOR;
	window_save_prompt_widgets[WIDX_TITLE].image = stringId;
	window_save_prompt_widgets[WIDX_LABEL].image = prompt_mode + STR_SAVE_BEFORE_LOADING;

	if (!gGeneral_config.confirmation_prompt) {
		/* game_load_or_quit_no_save_prompt() will exec requested task and close this window
		 * immediately again.
		 * TODO restructure these functions when we're sure game_load_or_quit_no_save_prompt()
		 * and game_load_or_quit() are not called by the original binary anymore.
		 */
		
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0D) {
			game_load_or_quit_no_save_prompt();
			return;
		}
		
		if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) != 0) {
			if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) != 1) {
				RCT2_CALLPROC_EBPSAFE(0x0066EE54);
				game_load_or_quit_no_save_prompt();
				return;
			} else {
				tutorial_stop();
				game_load_or_quit_no_save_prompt();
				return;
			}
		}
		
		if (RCT2_GLOBAL(0x009DEA66, uint16) < 3840) {
			game_load_or_quit_no_save_prompt();
			return;
		}
	}
	
}

/**
 *
 *  rct2: 0x0066DF17
 */
static void window_save_prompt_close()
{
	// Unpause the game
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) &= ~2;
	unpause_sounds();
	window_invalidate_by_class(WC_TOP_TOOLBAR);
}

/**
 *
 *  rct2: 0x0066DDF2
 */
static void window_save_prompt_mouseup()
{
	short widgetIndex;
	rct_window *w;
	short prompt_mode;

	window_widget_get_registers(w, widgetIndex);

	prompt_mode = RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16);

	if (prompt_mode == PM_QUIT) {
		switch (widgetIndex) {
		case WQIDX_OK:
			openrct2_finish();
			break;
		case WQIDX_CLOSE:
		case WQIDX_CANCEL:
			window_close(w);
			break;
		}
		return;
	} else {
		switch (widgetIndex) {
		case WIDX_SAVE:
			if (!save_game()) {
				// user pressed cancel
				window_close(w);
				return;
			}
			break;
		case WIDX_DONT_SAVE:
			game_load_or_quit_no_save_prompt();
			return;
		case WIDX_CLOSE:
		case WIDX_CANCEL:
			window_close(w);
			return;
		}
	}
	

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0D) {
		game_load_or_quit_no_save_prompt();
		return;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) != 0) {
		if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) != 1) {
			RCT2_CALLPROC_EBPSAFE(0x0066EE54);
			game_load_or_quit_no_save_prompt();
			return;
		} else {
			tutorial_stop();
			game_load_or_quit_no_save_prompt();
			return;
		}
	}

	if (RCT2_GLOBAL(0x009DEA66, uint16) < 3840) {
		game_load_or_quit_no_save_prompt();
		return;
	}
}

static void window_save_prompt_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
}