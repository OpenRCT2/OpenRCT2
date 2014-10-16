/*****************************************************************************
* Copyright (c) 2014 Ted John, Duncan Frost
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

/**
 * Text Input Window
 *
 * This is a new window created to replace the windows dialog box 
 * that is used for inputing new text for ride names and peep names.
 */

#include "../addresses.h"
#include "../config.h"
#include "../platform/osinterface.h"
#include "../interface/window.h"
#include "../interface/widget.h"
#include "../localisation/localisation.h"

#define WW 250
#define WH 90
#define MAX_TEXTINPUT 32

enum WINDOW_TEXT_INPUT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_CANCEL,
	WIDX_OKAY
};

// 0x9DE4E0
static rct_widget window_text_input_widgets[] = {
		{ WWT_FRAME, 0, 0, WW - 1, 0, WH - 1, STR_NONE, STR_NONE },
		{ WWT_CAPTION, 0, 1, WW - 2, 1, 14, STR_OPTIONS, STR_WINDOW_TITLE_TIP },
		{ WWT_CLOSEBOX, 0, WW - 13, WW - 3, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },
		{ WWT_DROPDOWN_BUTTON, 0, WW - 80, WW - 10, WH - 21, WH - 10, STR_CANCEL, STR_NONE },
		{ WWT_DROPDOWN_BUTTON, 0, 10, 80, WH - 21, WH - 10, STR_OK, STR_NONE },
		{ WIDGETS_END }
};

static void window_text_input_emptysub(){}
static void window_text_input_mouseup();
static void window_text_input_paint();
static void window_text_input_text(int key, rct_window* w);
static void window_text_input_update(rct_window* w);
static void window_text_input_close();

//0x9A3F7C
static void* window_text_input_events[] = {
	window_text_input_close,
	window_text_input_mouseup,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_update,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_text,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_emptysub,
	window_text_input_paint,
	window_text_input_emptysub
};

int input_text_description;
char text_input[MAX_TEXTINPUT] = { 0 };
rct_windowclass calling_class = 0;
rct_windownumber calling_number = 0;
int calling_widget = 0;

void window_text_input_open(rct_window* call_w, int call_widget, rct_string_id title, rct_string_id description, rct_string_id existing_text, uint32 existing_args){
	window_close_by_class(WC_TEXTINPUT);

	// Window will be in the center of the screen
	rct_window* w = window_create(
		(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) / 2) - WW / 2,
		(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) / 2) - WH / 2, 
		WW, 
		WH, 
		(uint32*)window_text_input_events, 
		WC_TEXTINPUT,
		0);

	w->widgets = window_text_input_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1<<WIDX_CANCEL) | (1<<WIDX_OKAY);

	window_text_input_widgets[WIDX_TITLE].image = title;

	// Clear the text input buffer
	memset(text_input, 0, MAX_TEXTINPUT);

	// Enter in the the text input buffer any existing
	// text.
	format_string(text_input, existing_text, &existing_args);

	// This is the text displayed above the input box
	input_text_description = description;

	// Save calling window details so that the information
	// can be passed back to the correct window & widget
	calling_class = call_w->classification;
	calling_number = call_w->number;
	calling_widget = call_widget;

	osinterface_start_text_input(text_input, MAX_TEXTINPUT);

	window_init_scroll_widgets(w);
	w->colours[0] = 7;
	w->colours[1] = 7;
	w->colours[2] = 1;
}

/**
*
*/
static void window_text_input_mouseup(){
	short widgetIndex;
	rct_window *w;
	rct_window *calling_w;
	window_widget_get_registers(w, widgetIndex);

	calling_w = window_find_by_number(calling_class, calling_number);

	switch (widgetIndex){
	case WIDX_CANCEL:
	case WIDX_CLOSE:
		osinterface_stop_text_input();
		// Pass back the text that has been entered.
		// ecx when zero means text input failed
		if (calling_w != NULL)
			RCT2_CALLPROC_X(calling_w->event_handlers[WE_TEXT_INPUT], 0, 0, 0, calling_widget, (int)calling_w, (int)text_input, 0);
		window_close(w);
		break;
	case WIDX_OKAY:
		osinterface_stop_text_input();
		// Pass back the text that has been entered.
		// ecx when none zero means text input success
		if (calling_w != NULL)
			RCT2_CALLPROC_X(calling_w->event_handlers[WE_TEXT_INPUT], 0, 0, 1, calling_widget, (int)calling_w, (int)text_input, 0);
		window_close(w);
	}
}

/**
*
*/
static void window_text_input_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	int y = w->y + 25;
	
	gfx_draw_string_centred(dpi, input_text_description, w->x + WW / 2, y, w->colours[1], 0);

	y += 25;

	gfx_fill_rect_inset(dpi, w->x + 10, y, w->x + WW - 10, y + 12, w->colours[1], 0x60);

	y += 1;
	gfx_draw_string(dpi, text_input, w->colours[2], w->x + 12, y);

	// Make a copy of the string for measuring the width.
	char temp_string[32] = { 0 }; 
	memcpy(temp_string, text_input, gTextInputCursorPosition);
	
	int x = w->x + 13 + gfx_get_string_width(temp_string);

	int width = 6;
	if ((uint32)gTextInputCursorPosition < strlen(text_input)){
		// Make a new 1 character wide string for measuring the width
		// of the character that the cursor is under.
		temp_string[1] = '\0';
		temp_string[0] = text_input[gTextInputCursorPosition];
		width = max(gfx_get_string_width(temp_string) - 2, 4);
	}

	// Draw the cursor
	y += 9;
	if (w->frame_no > 15){
		gfx_fill_rect(dpi, x, y, x + width, y, w->colours[1]);
	}
}


static void window_text_input_text(int key, rct_window* w){

	int text = key;
	char new_char = osinterface_scancode_to_rct_keycode(0xFF&key);

	// If the return button is pressed stop text input
	if (new_char == '\r'){
		osinterface_stop_text_input();
		window_close(w);
		rct_window* calling_w = window_find_by_number(calling_class, calling_number);
		// Pass back the text that has been entered.
		// ecx when none zero means text input success
		if (calling_w)
			RCT2_CALLPROC_X(calling_w->event_handlers[WE_TEXT_INPUT], 0, 0, 1, calling_widget, (int)calling_w, (int)text_input, 0);
	}
	
	window_invalidate(w);
}

void window_text_input_update(rct_window* w)
{
	rct_window* calling_w = window_find_by_number(calling_class, calling_number);
	// If the calling window is closed then close the text
	// input window.
	if (!calling_w){
		window_close(w);
	}

	// Used to blink the cursor.
	w->frame_no++;
	if (w->frame_no > 30) w->frame_no = 0;
	window_invalidate(w);
}

static void window_text_input_close(){
	// Make sure that we take it out of the text input
	// mode otherwise problems may occur.
	osinterface_stop_text_input();
}