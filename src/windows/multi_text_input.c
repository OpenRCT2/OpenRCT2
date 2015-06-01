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
#include "../platform/platform.h"
#include "../interface/window.h"
#include "../interface/widget.h"
#include "../localisation/localisation.h"

#define WW 250
#define WH 110

enum WINDOW_MULTI_TEXT_INPUT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_USERNAME,
	WIDX_PASSWORD,
	WIDX_CANCEL,
	WIDX_OKAY
};

// 0x9DE4E0
static rct_widget window_multi_text_input_widgets[] = {
	{ WWT_FRAME, 1, 0, WW - 1, 0, WH - 1, STR_NONE, STR_NONE },
	{ WWT_CAPTION, 1, 1, WW - 2, 1, 14, STR_OPTIONS, STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX, 1, WW - 13, WW - 3, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },
	{ WWT_SPINNER, 1, WW - 240, WW - 10, WH - 60, WH - 47, STR_NONE, STR_NONE },
	{ WWT_SPINNER, 1, WW - 240, WW - 10, WH - 41, WH - 30, STR_NONE, STR_NONE },
	{ WWT_DROPDOWN_BUTTON, 1, WW - 80, WW - 10, WH - 21, WH - 10, STR_CANCEL, STR_NONE },
	{ WWT_DROPDOWN_BUTTON, 1, 10, 80, WH - 21, WH - 10, STR_OK, STR_NONE },
	{ WIDGETS_END }
};

static void window_multi_text_input_emptysub(){}
static void window_multi_text_input_mouseup();
static void window_multi_text_input_paint();
static void window_multi_text_input_text(int key, rct_window* w);
static void window_multi_text_input_update();
static void window_multi_text_input_close();
static void window_multi_text_input_invalidate();

//0x9A3F7C
static void* window_text_input_events[] = {
	window_multi_text_input_close,
	window_multi_text_input_mouseup,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_update,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_text,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_emptysub,
	window_multi_text_input_invalidate,
	window_multi_text_input_paint,
	window_multi_text_input_emptysub
};

int input_text_description;
char multi_text_input[512] = { 0 };
char multi_text_input2[512] = { 0 };
rct_windowclass multi_calling_class = 0;
rct_windownumber multi_calling_number = 0;
int multi_calling_widget = 0;
int _multi_maxInputLength;
int _multi_maxInputLength2;
bool _multi_isPass;
bool _multi_isPass2;
int inInput;

void window_multi_text_input_open(
	rct_window* call_w, 
	int call_widget, 
	rct_string_id title, 
	rct_string_id description, 
	rct_string_id existing_text, 
	uint32 existing_args, 
	int maxLength, 
	bool isPass, 
	rct_string_id existing_text2, 
	uint32 existing_args2, 
	int maxLength2, 
	bool isPass2
) {
	_multi_maxInputLength = maxLength;
	_multi_maxInputLength2 = maxLength2;
	_multi_isPass = isPass;
	_multi_isPass2 = isPass2;

	window_close_by_class(WC_MULTI_TEXTINPUT);

	// Clear the text input buffer
	memset(multi_text_input, 0, maxLength);
	memset(multi_text_input2, 0, maxLength2);


	// Enter in the the text input buffer any existing
	// text.
	if (existing_text != (rct_string_id)STR_NONE)
		format_string(multi_text_input, existing_text, &existing_args);

	if (existing_text2 != (rct_string_id)STR_NONE)
		format_string(multi_text_input2, existing_text2, &existing_args2);

	// In order to prevent strings that exceed the maxLength
	// from crashing the game.
	multi_text_input[maxLength - 1] = '\0';
	multi_text_input2[maxLength2 - 1] = '\0';

	// This is the text displayed above the input box
	input_text_description = description;

	// Work out the existing size of the window
	char wrapped_string[512];
	char wrapped_string2[512];
	strcpy(wrapped_string, multi_text_input);
	strcpy(wrapped_string2, multi_text_input2);

	int no_lines = 0, font_height = 0;

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);
	gfx_wrap_string(wrapped_string2, WW - (24 + 13), &no_lines, &font_height);

	int height = no_lines * 10 + WH;

	// Window will be in the center of the screen
	rct_window* w = window_create_centred(
		WW,
		height,
		(uint32*)window_text_input_events,
		WC_MULTI_TEXTINPUT,
		WF_STICK_TO_FRONT
		);

	w->widgets = window_multi_text_input_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_USERNAME) | (1 << WIDX_PASSWORD) | (1 << WIDX_CANCEL) | (1 << WIDX_OKAY);

	window_multi_text_input_widgets[WIDX_TITLE].image = title;

	// Save calling window details so that the information
	// can be passed back to the correct window & widget
	multi_calling_class = call_w->classification;
	multi_calling_number = call_w->number;
	multi_calling_widget = call_widget;

	platform_start_text_input(multi_text_input, maxLength);
	inInput = 0;

	window_init_scroll_widgets(w);
	w->colours[0] = call_w->colours[0];
	w->colours[1] = call_w->colours[1];
	w->colours[2] = call_w->colours[2];
}

void window_multi_text_input_raw_open(
	rct_window* call_w, 
	int call_widget, 
	rct_string_id title, 
	rct_string_id description, 
	utf8string existing_text, 
	int maxLength, 
	bool isPass, 
	utf8string existing_text2, 
	int maxLength2, 
	bool isPass2
) {
	_multi_maxInputLength = maxLength;
	_multi_maxInputLength2 = maxLength2;
	_multi_isPass = isPass;
	_multi_isPass2 = isPass2;

	window_close_by_class(WC_MULTI_TEXTINPUT);

	// Clear the text input buffer
	memset(multi_text_input, 0, maxLength);
	memset(multi_text_input2, 0, maxLength2);

	// Enter in the the text input buffer any existing
	// text.
	if (existing_text != NULL)
		strncpy(multi_text_input, existing_text, maxLength);
	if (existing_text2 != NULL)
		strncpy(multi_text_input2, existing_text2, maxLength2);
	
	// In order to prevent strings that exceed the maxLength
	// from crashing the game.
	multi_text_input[maxLength - 1] = '\0';
	multi_text_input2[maxLength2 - 1] = '\0';


	// This is the text displayed above the input box
	input_text_description = description;

	// Work out the existing size of the window
	char wrapped_string[512];
	char wrapped_string2[512];
	strcpy(wrapped_string, multi_text_input);
	strcpy(wrapped_string2, multi_text_input2);

	int no_lines = 0, font_height = 0;

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);
	gfx_wrap_string(wrapped_string2, WW - (24 + 13), &no_lines, &font_height);

	int height = no_lines * 10 + WH;

	// Window will be in the center of the screen
	rct_window* w = window_create_centred(
		WW,
		height,
		(uint32*)window_text_input_events,
		WC_MULTI_TEXTINPUT,
		WF_STICK_TO_FRONT
		);

	w->widgets = window_multi_text_input_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_USERNAME) | (1 << WIDX_PASSWORD) | (1 << WIDX_CANCEL) | (1 << WIDX_OKAY);

	window_multi_text_input_widgets[WIDX_TITLE].image = title;

	// Save calling window details so that the information
	// can be passed back to the correct window & widget
	multi_calling_class = call_w->classification;
	multi_calling_number = call_w->number;
	multi_calling_widget = call_widget;

	platform_start_text_input(multi_text_input, maxLength);
	inInput = 0;

	window_init_scroll_widgets(w);
	w->colours[0] = call_w->colours[0];
	w->colours[1] = call_w->colours[1];
	w->colours[2] = call_w->colours[2];
}

/**
*
*/
static void window_multi_text_input_mouseup(){
	short widgetIndex;
	rct_window *w;
	rct_window *calling_w;
	window_widget_get_registers(w, widgetIndex);

	calling_w = window_find_by_number(multi_calling_class, multi_calling_number);

	switch (widgetIndex){
	case WIDX_CANCEL:
	case WIDX_CLOSE:
		platform_stop_text_input();
		// Pass back the text that has been entered.
		// ecx when zero means text input failed
		if (calling_w != NULL)
			window_event_textinput_call(calling_w, multi_calling_widget, NULL, NULL);
		window_close(w);
		break;
	case WIDX_OKAY:
		platform_stop_text_input();
		// Pass back the text that has been entered.
		// ecx when none zero means text input success
		if (calling_w != NULL)
			window_event_textinput_call(calling_w, multi_calling_widget, multi_text_input, multi_text_input2);
		window_close(w);
		break;
	case WIDX_USERNAME:
		platform_stop_text_input();
		platform_start_text_input(multi_text_input, _multi_maxInputLength);
		inInput = 0;
		break;
	case WIDX_PASSWORD:
		platform_stop_text_input();
		platform_start_text_input(multi_text_input2, _multi_maxInputLength2);
		inInput = 1;
		break;
	}
}

/**
*
*/
static void window_multi_text_input_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	int y = w->y + 25;

	int no_lines = 0;
	int font_height = 0;


	gfx_draw_string_centred(dpi, input_text_description, w->x + WW / 2, y, w->colours[1], &TextInputDescriptionArgs);

	y += 25;

	char wrapped_string[512];
	char wrapped_string2[512];
	char wrapped_string_pass[512];
	char wrapped_string2_pass[512];
	strcpy(wrapped_string, multi_text_input);
	strcpy(wrapped_string2, multi_text_input2);
	strcpy(wrapped_string_pass, multi_text_input);
	strcpy(wrapped_string2_pass, multi_text_input2);

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);
	gfx_wrap_string(wrapped_string2, WW - (24 + 13), &no_lines, &font_height);
	gfx_wrap_string(wrapped_string_pass, WW - (24 + 13), &no_lines, &font_height);
	gfx_wrap_string(wrapped_string2_pass, WW - (24 + 13), &no_lines, &font_height);

	//gfx_fill_rect_inset(dpi, w->x + 10, y, w->x + WW - 10, y + 10 * (no_lines + 1) + 3, w->colours[1], 0x60);

	y += 1;

	char* wrap_pointer = wrapped_string;
	char* wrap_pointer2 = wrapped_string2;
	char* wrap_pointer_pass = wrapped_string_pass;
	char* wrap_pointer2_pass = wrapped_string2_pass;
	int char_count = 0;
	int char_count2 = 0;
	uint8 cur_drawn = 0;
	uint8 cur_drawn2 = 0;

	int wrap_point_len = strlen(wrapped_string);
	int wrap_point2_len = strlen(wrapped_string2);

	if (_multi_isPass) {
		for (int i = 0; i < wrap_point_len; i++) {
			wrap_pointer_pass[i] = '*';
		}
	}
	if (_multi_isPass2) {
		for (int i = 0; i < wrap_point2_len; i++) {
			wrap_pointer2_pass[i] = '*';
		}
	}

	for (int line = 0; line <= no_lines; ++line){
		if (_multi_isPass) {
			gfx_draw_string(dpi, wrap_pointer_pass, w->colours[1], w->x + 12, y);
		}
		else {
			gfx_draw_string(dpi, wrap_pointer, w->colours[1], w->x + 12, y);
		}

		int string_length = get_string_length(wrap_pointer);

		if (inInput == 0) {
			if (!cur_drawn && (gTextInputCursorPosition <= char_count + string_length)){
				// Make a copy of the string for measuring the width.
				char temp_string[512] = { 0 };
				if (_multi_isPass) {
					memcpy(temp_string, wrap_pointer_pass, gTextInputCursorPosition - char_count);
				} else {
					memcpy(temp_string, wrap_pointer, gTextInputCursorPosition - char_count);
				}
				
				int cur_x = w->x + 13 + gfx_get_string_width(temp_string);

				int width = 6;
				if ((uint32)gTextInputCursorPosition < strlen(multi_text_input)){
					// Make a new 1 character wide string for measuring the width
					// of the character that the cursor is under.
					temp_string[1] = '\0';
					temp_string[0] = multi_text_input[gTextInputCursorPosition];
					width = max(gfx_get_string_width(temp_string) - 2, 4);
				}

				if (w->frame_no > 15){
					uint8 colour = RCT2_ADDRESS(0x0141FC48, uint8)[w->colours[1] * 8];
					gfx_fill_rect(dpi, cur_x, y + 9, cur_x + width, y + 9, colour + 5);
				}

				cur_drawn++;
			}
		}

		wrap_pointer += string_length + 1;

		if (multi_text_input[char_count + string_length] == ' ')char_count++;
		char_count += string_length;

		y += 18;
	}
	for (int line = 0; line <= no_lines; ++line){
		if (_multi_isPass2) {
			gfx_draw_string(dpi, wrap_pointer2_pass, w->colours[1], w->x + 12, y);
		} else {
			gfx_draw_string(dpi, wrap_pointer2, w->colours[1], w->x + 12, y);
		}

		int string_length = get_string_length(wrap_pointer2);

		if (inInput == 1) {
			if (!cur_drawn2 && (gTextInputCursorPosition <= char_count2 + string_length)){
				// Make a copy of the string for measuring the width.
				char temp_string[512] = { 0 };
				if (_multi_isPass2) {
					memcpy(temp_string, wrap_pointer2_pass, gTextInputCursorPosition - char_count2);
				} else {
					memcpy(temp_string, wrap_pointer2, gTextInputCursorPosition - char_count2);
				}
				int cur_x = w->x + 13 + gfx_get_string_width(temp_string);

				int width = 6;
				if ((uint32)gTextInputCursorPosition < strlen(multi_text_input2)){
					// Make a new 1 character wide string for measuring the width
					// of the character that the cursor is under.
					temp_string[1] = '\0';
					temp_string[0] = multi_text_input2[gTextInputCursorPosition];
					width = max(gfx_get_string_width(temp_string) - 2, 4);
				}

				if (w->frame_no > 15){
					uint8 colour = RCT2_ADDRESS(0x0141FC48, uint8)[w->colours[1] * 8];
					gfx_fill_rect(dpi, cur_x, y + 9, cur_x + width, y + 9, colour + 5);
				}

				cur_drawn2++;
			}
		}

		wrap_pointer2 += string_length + 1;

		if (multi_text_input2[char_count2 + string_length] == ' ')char_count2++;
		char_count2 += string_length;

		y += 10;
	}
}


static void window_multi_text_input_text(int key, rct_window* w){

	int text = key;
	char new_char = platform_scancode_to_rct_keycode(0xFF & key);

	// If the return button is pressed stop text input
	if (new_char == '\r'){
		if (inInput == 0){
			platform_stop_text_input();
			inInput = 1;
			platform_start_text_input(multi_text_input2, _multi_maxInputLength2);
		} else {
			platform_stop_text_input();
			window_close(w);
			rct_window* calling_w = window_find_by_number(multi_calling_class, multi_calling_number);
			// Pass back the text that has been entered.
			// ecx when none zero means text input success
			if (calling_w)
				window_event_textinput_call(calling_w, multi_calling_widget, multi_text_input, multi_text_input2);
		}
	}

	window_invalidate(w);
}

void window_multi_text_input_update()
{
	rct_window* w;

	window_get_register(w);

	rct_window* calling_w = window_find_by_number(multi_calling_class, multi_calling_number);
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

static void window_multi_text_input_close()
{
	// Make sure that we take it out of the text input
	// mode otherwise problems may occur.
	platform_stop_text_input();
}

static void window_multi_text_input_invalidate(){
	rct_window* w;

	window_get_register(w);

	// Work out the existing size of the window
	char wrapped_string[512];
	char wrapped_string2[512];
	strcpy(wrapped_string, multi_text_input);
	strcpy(wrapped_string2, multi_text_input2);

	int no_lines = 0, font_height = 0;

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);
	gfx_wrap_string(wrapped_string2, WW - (24 + 13), &no_lines, &font_height);

	int height = no_lines * 10 + WH;

	// Change window size if required.
	if (height != w->height) {
		window_invalidate(w);
		window_set_resize(w, WW, height, WW, height);
	}

	window_multi_text_input_widgets[WIDX_OKAY].top = height - 21;
	window_multi_text_input_widgets[WIDX_OKAY].bottom = height - 10;

	window_multi_text_input_widgets[WIDX_CANCEL].top = height - 21;
	window_multi_text_input_widgets[WIDX_CANCEL].bottom = height - 10;

	window_multi_text_input_widgets[WIDX_BACKGROUND].bottom = height - 1;
}