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

#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../util/util.h"
#include "../network/network.h"
#define LINE_HEIGHT 15
static char _password[33];

enum WINDOW_NETWORK_STATUS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PASSWORD
};

static rct_widget window_network_status_widgets[] = {
    { WWT_FRAME,			0,	0,		440,	0,		90,		0x0FFFFFFFF,				STR_NONE },									// panel / background
    { WWT_CAPTION,			0,	1,		438,	1,		14,		STR_NONE,					STR_WINDOW_TITLE_TIP },						// title bar
    { WWT_CLOSEBOX,			0,	427,	437,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },						// close x button
	{ WIDGETS_END },
};

#define STATUS_MAX_BUFFER 1024
static char window_network_status_text[STATUS_MAX_BUFFER];
int window_network_status_text_no_lines = 0;
static void window_network_status_onclose(rct_window *w);
static void window_network_status_mouseup(rct_window *w, int widgetIndex);
static void window_network_status_update(rct_window *w);
static void window_network_status_textinput(rct_window *w, int widgetIndex, char *text);
static void window_network_status_invalidate(rct_window *w);
static void window_network_status_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_network_status_events = {
	window_network_status_onclose,
	window_network_status_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_network_status_update,
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
	window_network_status_textinput,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_network_status_invalidate,
	window_network_status_paint,
	NULL
};

static close_callback _onClose = NULL;

void window_network_status_open(const char* text, close_callback onClose)
{
	_onClose = onClose;

	//Write FORMAT_BLACK to text
	int no_lines = 0, font_height = 0;
	char str[STATUS_MAX_BUFFER];
	char *buffer = str;
	str[0] = 0;
	buffer = utf8_write_codepoint(buffer, FORMAT_BLACK);
	safe_strcpy(buffer, text, STATUS_MAX_BUFFER - 5);
	
	//Store text into memory before determining line count.
	safe_strcpy(window_network_status_text, str, STATUS_MAX_BUFFER);
	
	//determine line count to help in sizing window
	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	gfx_wrap_string(str, 420 - (24 + 13), &no_lines, &font_height);

	int height = max(90, 75 + (no_lines * LINE_HEIGHT));
	int width = 420;

	// Check if window is already open. if it is, see if height needs resized based on line count
	rct_window *w = window_bring_to_front_by_class(WC_NETWORK_STATUS);
	if (w == NULL) {
		w = window_create_centred(width, height, &window_network_status_events, WC_NETWORK_STATUS, WF_10 | WF_TRANSPARENT);
		w->widgets = window_network_status_widgets;
		w->enabled_widgets = 1 << WIDX_CLOSE;
		window_init_scroll_widgets(w);
		w->no_list_items = 0;
		w->selected_list_item = -1;
	}
	else {
		width = w->width;
		window_set_resize(w, width, height, width, height);
		window_network_status_text_no_lines = no_lines;
	}
	
}

void window_network_status_close()
{
	_onClose = NULL;
	window_close_by_class(WC_NETWORK_STATUS);
}

void window_network_status_open_password()
{
	rct_window* window;
	window = window_bring_to_front_by_class(WC_NETWORK_STATUS);
	if (window == NULL)
		return;

	window_text_input_raw_open(window, WIDX_PASSWORD, STR_PASSWORD_REQUIRED, STR_PASSWORD_REQUIRED_DESC, _password, 32);
}

static void window_network_status_onclose(rct_window *w)
{
	if (_onClose != NULL) {
		_onClose();
	}
}

static void window_network_status_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	}
}

static void window_network_status_update(rct_window *w)
{
	widget_invalidate(w, WIDX_BACKGROUND);
}

static void window_network_status_textinput(rct_window *w, int widgetIndex, char *text)
{
	strcpy(_password, "");
	switch (widgetIndex) {
	case WIDX_PASSWORD:
		if (text != NULL)
			safe_strcpy(_password, text, sizeof(_password));
		break;
	}
	if (text == NULL) {
		network_shutdown_client();
	} else {
		network_send_password(_password);
	}
}

static void window_network_status_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	window_network_status_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_network_status_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	window_network_status_widgets[WIDX_TITLE].right = w->width - 2;
	window_network_status_widgets[WIDX_CLOSE].left = w->width - 2 - 0x0B;
	window_network_status_widgets[WIDX_CLOSE].right = w->width - 2 - 0x0B + 0x0A;
}

static void window_network_status_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	
	int x = w->x + (w->width / 2);
	int y = w->y + (w->height / 2);
	int width = w->width - (24 + 13);
	int colour = COLOUR_BLACK;

	//Below is a modified version of gfx_draw_string_centred_wrapped
	int font_height, line_height, line_width, line_y, num_lines;
	char* buffer = RCT2_ADDRESS(0x009C383D, char);

	if (gCurrentFontSpriteBase >= 0) {
		gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	}

	gfx_draw_string(dpi, buffer, colour, dpi->x, dpi->y);

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string_raw(buffer, window_network_status_text, NULL);

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

	// line_width unused here
	line_width = gfx_wrap_string(buffer, width, &num_lines, &font_height);
	line_height = LINE_HEIGHT;

	if (*buffer == FORMAT_OUTLINE) {
		line_height = line_height + 1;
	}

	font_height = (line_height / 2) * num_lines;
	line_y = y - font_height;

	gCurrentFontFlags = 0;

	for (int line = 0; line <= num_lines; ++line) {
		int half_width = gfx_get_string_width(buffer) / 2;
		gfx_draw_string(dpi, buffer, 0xFE, x - half_width, line_y);

		buffer = get_string_end(buffer) + 1;
		line_y += line_height;
	}
}
