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

/**
 * Text Input Window
 *
 * This is a new window created to replace the windows dialog box
 * that is used for inputing new text for ride names and peep names.
 */

#include "../config/Config.h"
#include "../platform/platform.h"
#include "../interface/window.h"
#include "../interface/widget.h"
#include "../localisation/localisation.h"
#include "../util/util.h"

#define WW 250
#define WH 90

enum WINDOW_TEXT_INPUT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_CANCEL,
	WIDX_OKAY
};

// 0x9DE4E0
static rct_widget window_text_input_widgets[] = {
		{ WWT_FRAME, 1, 0, WW - 1, 0, WH - 1, STR_NONE, STR_NONE },
		{ WWT_CAPTION, 1, 1, WW - 2, 1, 14, STR_OPTIONS, STR_WINDOW_TITLE_TIP },
		{ WWT_CLOSEBOX, 1, WW - 13, WW - 3, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },
		{ WWT_DROPDOWN_BUTTON, 1, WW - 80, WW - 10, WH - 21, WH - 10, STR_CANCEL, STR_NONE },
		{ WWT_DROPDOWN_BUTTON, 1, 10, 80, WH - 21, WH - 10, STR_OK, STR_NONE },
		{ WIDGETS_END }
};

static void window_text_input_close(rct_window *w);
static void window_text_input_mouseup(rct_window *w, sint32 widgetIndex);
static void window_text_input_update7(rct_window *w);
static void window_text_input_text(sint32 key, rct_window* w);
static void window_text_input_invalidate(rct_window *w);
static void window_text_input_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void draw_ime_composition(rct_drawpixelinfo * dpi, int cursorX, int cursorY);

//0x9A3F7C
static rct_window_event_list window_text_input_events = {
	window_text_input_close,
	window_text_input_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_text_input_update7,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_text_input_invalidate,
	window_text_input_paint,
	NULL
};

rct_string_id input_text_description;
char text_input[TEXT_INPUT_SIZE] = { 0 };
rct_windowclass calling_class = 0;
rct_windownumber calling_number = 0;
sint32 calling_widget = 0;
sint32 _maxInputLength;

void window_text_input_open(rct_window* call_w, sint32 call_widget, rct_string_id title, rct_string_id description, rct_string_id existing_text, uintptr_t existing_args, sint32 maxLength)
{
	_maxInputLength = maxLength;

	window_close_by_class(WC_TEXTINPUT);

	// Clear the text input buffer
	memset(text_input, 0, maxLength);

	// Enter in the text input buffer any existing
	// text.
	if (existing_text != STR_NONE)
		format_string(text_input, maxLength, existing_text, &existing_args);

	utf8_remove_format_codes(text_input, false);

	// This is the text displayed above the input box
	input_text_description = description;

	// Work out the existing size of the window
	char wrapped_string[TEXT_INPUT_SIZE];
	safe_strcpy(wrapped_string, text_input, TEXT_INPUT_SIZE);

	sint32 no_lines = 0, font_height = 0;

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);

	sint32 height = no_lines * 10 + WH;

	// Window will be in the center of the screen
	rct_window* w = window_create_centred(
		WW,
		height,
		&window_text_input_events,
		WC_TEXTINPUT,
		WF_STICK_TO_FRONT
	);

	w->widgets = window_text_input_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1<<WIDX_CANCEL) | (1<<WIDX_OKAY);

	window_text_input_widgets[WIDX_TITLE].text = title;

	// Save calling window details so that the information
	// can be passed back to the correct window & widget
	calling_class = call_w->classification;
	calling_number = call_w->number;
	calling_widget = call_widget;

	platform_start_text_input(text_input, maxLength);

	window_init_scroll_widgets(w);
	w->colours[0] = call_w->colours[0];
	w->colours[1] = call_w->colours[1];
	w->colours[2] = call_w->colours[2];
}

void window_text_input_raw_open(rct_window* call_w, sint32 call_widget, rct_string_id title, rct_string_id description, utf8string existing_text, sint32 maxLength)
{
	_maxInputLength = maxLength;

	window_close_by_class(WC_TEXTINPUT);

	// Clear the text input buffer
	memset(text_input, 0, maxLength);

	// Enter in the text input buffer any existing
	// text.
	if (existing_text != NULL)
		safe_strcpy(text_input, existing_text, maxLength);

	// In order to prevent strings that exceed the maxLength
	// from crashing the game.
	text_input[maxLength - 1] = '\0';

	// This is the text displayed above the input box
	input_text_description = description;

	// Work out the existing size of the window
	char wrapped_string[TEXT_INPUT_SIZE];
	safe_strcpy(wrapped_string, text_input, TEXT_INPUT_SIZE);

	sint32 no_lines = 0, font_height = 0;

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);

	sint32 height = no_lines * 10 + WH;

	// Window will be in the center of the screen
	rct_window* w = window_create_centred(
		WW,
		height,
		&window_text_input_events,
		WC_TEXTINPUT,
		WF_STICK_TO_FRONT
		);

	w->widgets = window_text_input_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_CANCEL) | (1 << WIDX_OKAY);

	window_text_input_widgets[WIDX_TITLE].text = title;

	// Save calling window details so that the information
	// can be passed back to the correct window & widget
	calling_class = call_w->classification;
	calling_number = call_w->number;
	calling_widget = call_widget;

	platform_start_text_input(text_input, maxLength);

	window_init_scroll_widgets(w);
	w->colours[0] = call_w->colours[0];
	w->colours[1] = call_w->colours[1];
	w->colours[2] = call_w->colours[2];
}

/**
*
*/
static void window_text_input_mouseup(rct_window *w, sint32 widgetIndex)
{
	rct_window *calling_w;

	calling_w = window_find_by_number(calling_class, calling_number);
	switch (widgetIndex){
	case WIDX_CANCEL:
	case WIDX_CLOSE:
		platform_stop_text_input();
		// Pass back the text that has been entered.
		// ecx when zero means text input failed
		if (calling_w != NULL)
			window_event_textinput_call(calling_w, calling_widget, NULL);
		window_close(w);
		break;
	case WIDX_OKAY:
		platform_stop_text_input();
		// Pass back the text that has been entered.
		// ecx when none zero means text input success
		if (calling_w != NULL)
			window_event_textinput_call(calling_w, calling_widget, text_input);
		window_close(w);
	}
}

/**
*
*/
static void window_text_input_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	sint32 y = w->y + 25;

	sint32 no_lines = 0;
	sint32 font_height = 0;


	gfx_draw_string_centred(dpi, input_text_description, w->x + WW / 2, y, w->colours[1], &TextInputDescriptionArgs);

	y += 25;

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	gCurrentFontFlags = 0;

	char wrapped_string[TEXT_INPUT_SIZE];
	safe_strcpy(wrapped_string, text_input, TEXT_INPUT_SIZE);

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);

	gfx_fill_rect_inset(dpi, w->x + 10, y, w->x + WW - 10, y + 10 * (no_lines + 1) + 3, w->colours[1], INSET_RECT_F_60);

	y += 1;

	char* wrap_pointer = wrapped_string;
	size_t char_count = 0;
	uint8 cur_drawn = 0;

	sint32 cursorX = 0, cursorY = 0;
	for (sint32 line = 0; line <= no_lines; line++) {
		gfx_draw_string(dpi, wrap_pointer, w->colours[1], w->x + 12, y);

		size_t string_length = get_string_size(wrap_pointer) - 1;

		if (!cur_drawn && (gTextInput.selection_offset <= char_count + string_length)) {
			// Make a copy of the string for measuring the width.
			char temp_string[TEXT_INPUT_SIZE] = { 0 };
			memcpy(temp_string, wrap_pointer, gTextInput.selection_offset - char_count);
			cursorX = w->x + 13 + gfx_get_string_width(temp_string);
			cursorY = y;

			sint32 width = 6;
			if (gTextInput.selection_offset < strlen(text_input)){
				// Make a 1 utf8-character wide string for measuring the width
				// of the currently selected character.
				utf8 tmp[5] = { 0 }; // This is easier than setting temp_string[0..5]
				uint32 codepoint = utf8_get_next(text_input + gTextInput.selection_offset, NULL);
				utf8_write_codepoint(tmp, codepoint);
				width = max(gfx_get_string_width(tmp) - 2, 4);
			}

			if (w->frame_no > 15){
				uint8 colour = ColourMapA[w->colours[1]].mid_light;
				gfx_fill_rect(dpi, cursorX, y + 9, cursorX + width, y + 9, colour + 5);
			}

			cur_drawn++;
		}

		wrap_pointer += string_length + 1;

		if (text_input[char_count + string_length] == ' ')char_count++;
		char_count += string_length;

		y += 10;
	}

	if (!cur_drawn) {
		cursorX = gLastDrawStringX;
		cursorY = y - 10;
	}

	// IME composition
	if (gTextInputCompositionActive) {
		draw_ime_composition(dpi, cursorX, cursorY);
	}
}

void window_text_input_key(rct_window* w, sint32 key)
{
	char new_char = platform_scancode_to_rct_keycode(0xFF&key);

	// If the return button is pressed stop text input
	if (new_char == '\r'){
		platform_stop_text_input();
		window_close(w);
		rct_window* calling_w = window_find_by_number(calling_class, calling_number);
		// Pass back the text that has been entered.
		// ecx when none zero means text input success
		if (calling_w)
			window_event_textinput_call(calling_w, calling_widget, text_input);
	}

	window_invalidate(w);
}

void window_text_input_update7(rct_window *w)
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

static void window_text_input_close(rct_window *w)
{
	// Make sure that we take it out of the text input
	// mode otherwise problems may occur.
	platform_stop_text_input();
}

static void window_text_input_invalidate(rct_window *w)
{
	// Work out the existing size of the window
	char wrapped_string[TEXT_INPUT_SIZE];
	safe_strcpy(wrapped_string, text_input, TEXT_INPUT_SIZE);

	sint32 no_lines = 0, font_height = 0;

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);

	sint32 height = no_lines * 10 + WH;

	// Change window size if required.
	if (height != w->height) {
		window_invalidate(w);
		window_set_resize(w, WW, height, WW, height);
	}

	window_text_input_widgets[WIDX_OKAY].top = height - 21;
	window_text_input_widgets[WIDX_OKAY].bottom = height - 10;

	window_text_input_widgets[WIDX_CANCEL].top = height - 21;
	window_text_input_widgets[WIDX_CANCEL].bottom = height - 10;

	window_text_input_widgets[WIDX_BACKGROUND].bottom = height - 1;
}

static void draw_ime_composition(rct_drawpixelinfo * dpi, int cursorX, int cursorY)
{
	int compositionWidth = gfx_get_string_width(gTextInputComposition);
	int x = cursorX - (compositionWidth / 2);
	int y = cursorY + 13;
	int width = compositionWidth;
	int height = 10;

	gfx_fill_rect(dpi, x - 1, y - 1, x + width + 1, y + height + 1, 12);
	gfx_fill_rect(dpi, x, y, x + width, y + height, 0);
	gfx_draw_string(dpi, gTextInputComposition, COLOUR_DARK_GREEN, x, y);
}
