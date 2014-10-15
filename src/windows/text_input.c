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
};

// 0x9DE4E0
static rct_widget window_text_input_widgets[] = {
		{ WWT_FRAME, 0, 0, WW - 1, 0, WH - 1, STR_NONE, STR_NONE },
		{ WWT_CAPTION, 0, 1, WW - 2, 1, 14, STR_OPTIONS, STR_WINDOW_TITLE_TIP },
		{ WWT_CLOSEBOX, 0, WW - 13, WW - 3, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },
		{ WIDGETS_END }
};

static void window_text_input_emptysub(){}
static void window_text_input_mouseup();
static void window_text_input_paint();
static void window_text_input_text(int key, rct_window* w);
static void window_text_input_update(rct_window* w);

//0x9A3F7C
static void* window_text_input_events[] = {
	window_text_input_emptysub,
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

int string_description;
char text_input[MAX_TEXTINPUT] = { 0 };
rct_window* calling_w = NULL;
int calling_widget = 0;
char current_mode = 0;

void window_text_input_open(rct_window* call_w, int call_widget, uint16 title, uint16 description, rct_string_id string_id, uint32 args){
	window_close_by_id(113, 0);

	rct_window* w = window_create(
		(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) / 2) - WW / 2,
		(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) / 2) - WH / 2, 
		WW, 
		WH, 
		(uint32*)window_text_input_events, 
		113, 
		0);

	w->widgets = window_text_input_widgets;
	w->enabled_widgets = (1 << 2);

	window_text_input_widgets[WIDX_TITLE].image = title;

	format_string(text_input, string_id, &args);
	string_description = description;
	current_mode = 0;
	calling_w = call_w;
	calling_widget = call_widget;
	osinterface_start_text_input(text_input, MAX_TEXTINPUT);

	window_init_scroll_widgets(w);
	w->colours[0] = 7;
	w->colours[1] = 7;
	w->colours[2] = 1;
}

/**
*
*  rct2: 0x006E3AE0
*/
static void window_text_input_mouseup(){
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex){
	case WIDX_CLOSE:
		osinterface_stop_text_input();
		if (calling_w != NULL)
			RCT2_CALLPROC_X(calling_w->event_handlers[WE_TEXT_INPUT], 0, 0, 0, calling_widget, (int)calling_w, (int)text_input, 0);
		window_close(w);
	}
}

/**
*
*  rct2: 0x006E3A9F
*/
static void window_text_input_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	int y = w->y + 25;
	
	gfx_draw_string_centred(dpi, string_description, w->x + 4 + WW / 2, y, w->colours[1], 0);

	y += 25;

	gfx_fill_rect_inset(dpi, w->x + 4, y, w->x + WW - 4, y + 12, w->colours[1], 0x60);

	y += 1;
	gfx_draw_string(dpi, text_input, w->colours[2], w->x + 6, y);

	char temp_string[32] = { 0 };
	memcpy(temp_string, text_input, gTextInputCursorPosition);
	
	int x = w->x + 7 + gfx_get_string_width(temp_string);

	int width = 6;
	if (gTextInputCursorPosition < strlen(text_input)){
		temp_string[1] = '\0';
		temp_string[0] = text_input[gTextInputCursorPosition];
		width = max(gfx_get_string_width(temp_string) - 2, 4);
	}
	y += 9;
	if ((w->frame_no > 15) && current_mode){
		gfx_fill_rect(dpi, x, y, x + width, y, w->colours[1]);
	}
}


static void window_text_input_text(int key, rct_window* w){

	int text = key;
	char new_char = osinterface_scancode_to_rct_keycode(0xFF&key);
	if (!current_mode){
		// Delete the existing text on first key down
		current_mode++;
		memset(text_input+1, 0, MAX_TEXTINPUT - 1);
	}
	// If the return button is pressed stop text input
	if (new_char == '\r'){
		osinterface_stop_text_input();
		window_close(w);
		RCT2_CALLPROC_X(calling_w->event_handlers[WE_TEXT_INPUT], 0, 0, 1, calling_widget, (int)calling_w, (int)text_input, 0);
	}
	
	window_invalidate(w);
}

void window_text_input_update(rct_window* w)
{
	w->frame_no++;
	if (w->frame_no > 30) w->frame_no = 0;
	window_invalidate(w);
}