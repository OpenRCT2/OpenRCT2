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

#include "addresses.h"
#include "game.h"
#include "rct2.h"
#include "string_ids.h"
#include "sprites.h"
#include "tutorial.h"
#include "widget.h"
#include "window.h"
#include "audio.h"

enum WINDOW_SAVE_PROMPT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_3,
	WIDX_SAVE,
	WIDX_DONT_SAVE,
	WIDX_CANCEL
};

static rct_widget window_save_prompt_widgets[] = {
	{ WWT_FRAME,			0,	0,		259,	0,	49,	-1,							STR_NONE },					// panel / background
	{ WWT_CAPTION,			0,	1,		258,	1,	14,	0,							STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0,	247,	257,	2,	13,	824,						STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_12,				0,	2,		257,	19,	30,	0,							STR_NONE },					// 
	{ WWT_DROPDOWN_BUTTON,	0,	8,		85,		35,	46,	STR_SAVE_PROMPT_SAVE,		STR_NONE },					// save
	{ WWT_DROPDOWN_BUTTON,	0,	91,		168,	35,	46,	STR_SAVE_PROMPT_DONT_SAVE,	STR_NONE },					// don't save
	{ WWT_DROPDOWN_BUTTON,	0,	174,	251,	35,	46,	STR_SAVE_PROMPT_CANCEL,		STR_NONE },					// cancel
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
	int stringId;
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_id(WC_SAVE_PROMPT, 0);
	if (window == NULL) {
		window = window_create(
			(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) / 2) - 130,
			max(28, (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) / 2) - 25),
			260,
			50,
			(uint32*)window_save_prompt_events,
			WC_SAVE_PROMPT,
			0
		);

		window->widgets = window_save_prompt_widgets;
		window->enabled_widgets =
			(1 << WIDX_CLOSE) |
			(1 << WIDX_SAVE) |
			(1 << WIDX_DONT_SAVE) |
			(1 << WIDX_CANCEL);
		window_init_scroll_widgets(window);
		window->colours[0] = 154;
		window->flags |= WF_TRANSPARENT;

		// Pause the game
		RCT2_GLOBAL(0x009DEA6E, uint8) |= 2;
		pause_sounds();
		window_invalidate_by_id(0x80 | WC_TOP_TOOLBAR, 0);
	}

	stringId = RCT2_GLOBAL(0x009A9802, uint16) + STR_LOAD_GAME;
	if (stringId == STR_LOAD_GAME && RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
		stringId = STR_LOAD_LANDSCAPE;
	if (stringId == STR_QUIT_GAME && RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
		stringId = STR_QUIT_SCENARIO_EDITOR;
	window_save_prompt_widgets[WIDX_TITLE].image = stringId;
	window_save_prompt_widgets[WIDX_3].image = RCT2_GLOBAL(0x009A9802, uint16) + STR_SAVE_BEFORE_LOADING;

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

/**
 *
 *  rct2: 0x0066DF17
 */
static void window_save_prompt_close()
{
	// Unpause the game
	RCT2_GLOBAL(0x009DEA6E, uint8) &= ~2;
	unpause_sounds();
	window_invalidate_by_id(0x80 | WC_TOP_TOOLBAR, 0);
}

/**
 *
 *  rct2: 0x0066DDF2
 */
static void window_save_prompt_mouseup()
{
	short widgetIndex;
	rct_window *w;

	#ifdef _MSC_VER
	__asm mov widgetIndex, dx
	#else
	__asm__ ( ".intel_syntax noprefix\n mov %[widgetIndex], dx " : [widgetIndex] "+m" (widgetIndex) );
	#endif

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( ".intel_syntax noprefix\n mov %[w], esi " : [w] "+m" (w) );
	#endif


	// TODO
}

static void window_save_prompt_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( ".intel_syntax noprefix\n mov %[w], esi " : [w] "+m" (w) );
	#endif

	#ifdef _MSC_VER
	__asm mov dpi, edi
	#else
	__asm__ ( ".intel_syntax noprefix\n mov %[dpi], edi " : [dpi] "+m" (dpi) );
	#endif


	window_draw_widgets(w, dpi);
}