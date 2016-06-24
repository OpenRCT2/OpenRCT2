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

#include "../addresses.h"
#include "../audio/audio.h"
#include "../audio/mixer.h"
#include "../localisation/localisation.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "chat.h"
#include "../util/util.h"
#include "../interface/themes.h"
#include "../interface/window.h"
#include "../interface/widget.h"
#include "../interface/colour.h"
#include "../config.h"
#include "../input.h"

#define CHAT_HISTORY_SIZE 100
#define CHAT_INPUT_SIZE 256

#define CHAT_MIN_WIDTH 300
#define CHAT_MIN_HEIGHT 100
#define CHAT_MAX_WIDTH 700
#define CHAT_MAX_HEIGHT 300

bool gChatOpen = false;
char _chatCurrentLine[200]; //Limit 200 characters
char _chatHistory[CHAT_HISTORY_SIZE][CHAT_INPUT_SIZE];
uint32 _chatHistoryTime[CHAT_HISTORY_SIZE];
unsigned int _chatHistoryIndex = 0;
uint32 _chatCaretTicks = 0;
int _chatLeft;
int _chatTop;
int _chatRight;
int _chatBottom;
int _chatWidth;
int _chatHeight;

static const char* chat_history_get(unsigned int index);
static uint32 chat_history_get_time(unsigned int index);
static void chat_clear_input();

enum WINDOW_GUEST_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_RESIZE,
	WIDX_CHAT_INPUT,
	WIDX_CHAT_HISTORY
};

static rct_widget window_chat_widgets[] = {
	{ WWT_FRAME,		0, 0,	CHAT_MIN_WIDTH - 1,		0,		CHAT_MIN_HEIGHT - 1,		STR_NONE,		STR_NONE },					// Background
	{ WWT_CAPTION,		0, 1,	CHAT_MIN_WIDTH - 2,		1,		10,							STR_NONE,		STR_CHAT_TITLE_TIP },		// Title
	{ WWT_RESIZE,		0, 0,	CHAT_MIN_WIDTH - 1,		11,		CHAT_MIN_HEIGHT - 1,		0x0FFFFFFFF,	STR_NONE },					// Resize
	{ WWT_TEXT_BOX,		0, 2,	CHAT_MIN_WIDTH - 3,		12,		27,							STR_NONE,		STR_NONE },
	{ WWT_SCROLL,		0, 3,	CHAT_MIN_WIDTH - 4,		28,		CHAT_MIN_HEIGHT - 2,		2,				STR_NONE },					// Chat history
	{ WIDGETS_END },
};

static void window_chat_mouseup(rct_window *w, int widgetIndex);
static void window_chat_resize(rct_window *w);
static void window_chat_update(rct_window *w);
static void window_chat_invalidate(rct_window *w);
static void window_chat_textinput(rct_window *w, int widgetIndex, char *text);
static void window_chat_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_chat_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_chat_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_chat_events = {
	NULL,
	window_chat_mouseup,
	window_chat_resize,
	NULL,
	NULL,
	NULL,
	window_chat_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_chat_scrollgetsize,
	NULL,
	NULL,
	NULL,
	window_chat_textinput,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_chat_invalidate,
	window_chat_paint,
	window_chat_scrollpaint
};

void chat_open()
{
	gChatOpen = true;
	//platform_start_text_input(_chatCurrentLine, sizeof(_chatCurrentLine));}
	rct_window *chatWindow = window_find_by_class(WC_CHAT);
	if (chatWindow != NULL) {
		window_start_textbox(chatWindow, WIDX_CHAT_INPUT, 1170, (uint32)_chatCurrentLine, 200);
	}
}

void chat_close()
{
	gChatOpen = false;
	rct_window *chatWindow = window_find_by_class(WC_CHAT);
	if (chatWindow != NULL) {
		window_cancel_textbox();
	}
}

void chat_toggle()
{
	if (gChatOpen) {
		chat_close();
	} else {
		chat_open();
	}
}

void chat_init()
{
	memset(_chatHistory, 0, sizeof(_chatHistory));
	memset(_chatHistoryTime, 0, sizeof(_chatHistoryTime));
}

void chat_update()
{
	// Flash the caret
	_chatCaretTicks = (_chatCaretTicks + 1) % 30;
}

void chat_draw(rct_drawpixelinfo * dpi)
{
	if (network_get_mode() == NETWORK_MODE_NONE || network_get_status() != NETWORK_STATUS_CONNECTED || network_get_authstatus() != NETWORK_AUTH_OK) {
		gChatOpen = false;
		return;
	}

	//TEST
	if (window_find_by_class(WC_CHAT) == NULL) {
		rct_window *chatWindow = window_create(25, gScreenHeight - 200, 500, 150, &window_chat_events, WC_CHAT, WF_RESIZABLE | WF_NO_AUTO_CLOSE);
		window_chat_widgets[WIDX_CHAT_INPUT].image = (uint32)_chatCurrentLine;
		chatWindow->widgets = window_chat_widgets;
		chatWindow->enabled_widgets = (1 << WIDX_CHAT_INPUT);
		window_init_scroll_widgets(chatWindow);
		chatWindow->min_width = CHAT_MIN_WIDTH;
		chatWindow->min_height = CHAT_MIN_HEIGHT;
		chatWindow->max_width = CHAT_MAX_WIDTH;
		chatWindow->max_height = CHAT_MAX_HEIGHT;
	}
	return;

	_chatLeft = 10;
	_chatRight = gScreenWidth - 10;
	_chatBottom = gScreenHeight - 45;
	_chatTop = _chatBottom - 10;

	//Max width
	if (_chatRight > 600) {
		_chatRight = 600;
	}

	_chatWidth = _chatRight - _chatLeft;

	char lineBuffer[CHAT_INPUT_SIZE + 10];
	char* lineCh = lineBuffer;
	char* inputLine = _chatCurrentLine;
	int inputLineHeight = 10;
	uint8 chatBackgroundColor = theme_get_colour(WC_CHAT, 0);

	if (gChatOpen) {
		inputLineHeight = chat_string_wrapped_get_height((void*)&inputLine, _chatWidth - 10);
		_chatTop -= inputLineHeight;

		for (int i = 0; i < CHAT_HISTORY_SIZE; i++) {
			if (strlen(chat_history_get(i)) == 0) {
				continue;
			}

			safe_strcpy(lineBuffer, chat_history_get(i), CHAT_INPUT_SIZE + 10);

			int lineHeight = chat_string_wrapped_get_height((void*)&lineCh, _chatWidth - 10);
			_chatTop -= (lineHeight + 5);
		}

		_chatHeight = _chatBottom - _chatTop;

		if (_chatTop < 50) {
			_chatTop = 50;
		}else if (_chatHeight < 150) { //Min width
			_chatTop = _chatBottom - 150;
			_chatHeight = 150;
		}

		gfx_set_dirty_blocks(_chatLeft, _chatTop - 5, _chatRight, _chatBottom + 5); //Background area + Textbox
		gfx_fill_rect(
			dpi,
			_chatLeft,
			_chatTop - 5,
			_chatRight,
			_chatBottom + 5,
			0x2000000 | 51 //gray background, this makes the chat more opaque
		);
		gfx_fill_rect_inset(
			dpi, 
			_chatLeft, 
			_chatTop - 5, 
			_chatRight, 
			_chatBottom + 5, 
			chatBackgroundColor,
			0x10 //No fill
		);
		gfx_fill_rect_inset(
			dpi,
			_chatLeft + 1,
			_chatTop - 4,
			_chatRight - 1,
			_chatBottom - inputLineHeight - 6,
			chatBackgroundColor,
			0x20 //Pressed
		);
		gfx_fill_rect_inset(
			dpi, 
			_chatLeft + 1,
			_chatBottom - inputLineHeight - 5,
			_chatRight - 1, 
			_chatBottom + 4,
			chatBackgroundColor,
			0x20 //Pressed
		); //Textbox
	}
	
	int x = _chatLeft + 5;
	int y = _chatBottom - inputLineHeight - 20;
	int stringHeight = 0;

	for (int i = 0; i < CHAT_HISTORY_SIZE; i++, y -= stringHeight) {
		if (!gChatOpen && SDL_TICKS_PASSED(SDL_GetTicks(), chat_history_get_time(i) + 10000)) {
			break;
		}

		safe_strcpy(lineBuffer, chat_history_get(i), CHAT_INPUT_SIZE + 10);

		stringHeight = chat_history_draw_string(dpi, (void*) &lineCh, x, y, _chatWidth - 10) + 5;
		gfx_set_dirty_blocks(x, y - stringHeight, x + _chatWidth, y + 20);

		if ((y - stringHeight) < 50) {
			break;
		}
	}
	
	if (gChatOpen) {
		lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
		lineCh = utf8_write_codepoint(lineCh, FORMAT_CELADON);

		safe_strcpy(lineCh, _chatCurrentLine, sizeof(_chatCurrentLine));
		y = _chatBottom - inputLineHeight - 5;

		int inputLineHeight = gfx_draw_string_left_wrapped(dpi, (void*)&lineCh, x, y + 3, _chatWidth - 10, STR_STRING, 255);
		gfx_set_dirty_blocks(x, y, x + _chatWidth, y + inputLineHeight + 15);
		
		//TODO: Show caret if the input text have multiple lines
		if (_chatCaretTicks < 15 && gfx_get_string_width(lineBuffer) < (_chatWidth - 10)) {
			memcpy(lineBuffer, _chatCurrentLine, gTextInput.selection_offset);
			lineBuffer[gTextInput.selection_offset] = 0;
			int caretX = x + gfx_get_string_width(lineBuffer);
			int caretY = y + 14;

			gfx_fill_rect(dpi, caretX, caretY, caretX + 6, caretY + 1, 0x38);
		}
	}
}

void chat_history_add(const char *src)
{
	int index = _chatHistoryIndex % CHAT_HISTORY_SIZE;
	memset(_chatHistory[index], 0, CHAT_INPUT_SIZE);
	memcpy(_chatHistory[index], src, min(strlen(src), CHAT_INPUT_SIZE - 1));
	_chatHistoryTime[index] = SDL_GetTicks();
	_chatHistoryIndex++;
	Mixer_Play_Effect(SOUND_NEWS_ITEM, 0, SDL_MIX_MAXVOLUME, 0, 1.5f, true);
	network_append_chat_log(src);
}

void chat_input(int c)
{
	switch (c) {
	case SDL_SCANCODE_RETURN:
		if (strlen(_chatCurrentLine) > 0) {
			network_send_chat(_chatCurrentLine);
		}
		chat_clear_input();
		chat_close();
		return;
	case SDL_SCANCODE_ESCAPE:
		chat_close();
		return;
	}

	rct_window *chatWindow = window_find_by_class(WC_CHAT);
	if (chatWindow != NULL)
		window_text_input_key(chatWindow, c);
	return;
}

static const char* chat_history_get(unsigned int index)
{
	return _chatHistory[(_chatHistoryIndex + CHAT_HISTORY_SIZE - index - 1) % CHAT_HISTORY_SIZE];
}

static uint32 chat_history_get_time(unsigned int index)
{
	return _chatHistoryTime[(_chatHistoryIndex + CHAT_HISTORY_SIZE - index - 1) % CHAT_HISTORY_SIZE];
}

static void chat_clear_input()
{
	_chatCurrentLine[0] = 0;
}

// This method is the same as gfx_draw_string_left_wrapped.
// But this adjusts the initial Y coordinate depending of the number of lines.
int chat_history_draw_string(rct_drawpixelinfo *dpi, void *args, int x, int y, int width)
{
	int fontSpriteBase, lineHeight, lineY, numLines;
	
	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

	char* buffer = RCT2_ADDRESS(0x009C383D, char);
	gfx_draw_string(dpi, buffer, 255, dpi->x, dpi->y);
	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, STR_STRING, args);

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	gfx_wrap_string(buffer, width, &numLines, &fontSpriteBase);
	lineHeight = font_get_line_height(fontSpriteBase);

	gCurrentFontFlags = 0;

	int expectedY = y - (numLines * lineHeight);

	lineY = y;
	for (int line = 0; line <= numLines; ++line) {
		gfx_draw_string(dpi, buffer, 0xFE, x, lineY - (numLines * lineHeight));
		buffer = get_string_end(buffer) + 1;
		lineY += lineHeight;
	}
	return lineY - y;
}

// Wrap string without drawing, useful to get the height of a wrapped string.
// Almost the same as gfx_draw_string_left_wrapped
int chat_string_wrapped_get_height(void *args, int width)
{
	int fontSpriteBase, lineHeight, lineY, numLines;

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

	char* buffer = RCT2_ADDRESS(0x009C383D, char);
	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, STR_STRING, args);

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	gfx_wrap_string(buffer, width, &numLines, &fontSpriteBase);
	lineHeight = font_get_line_height(fontSpriteBase);
	
	gCurrentFontFlags = 0;

	lineY = 0;
	for (int line = 0; line <= numLines; ++line) {
		buffer = get_string_end(buffer) + 1;
		lineY += lineHeight;
	}

	return lineY;
}

static void window_chat_paint(rct_window *w, rct_drawpixelinfo *dpi) 
{
	window_draw_widgets(w, dpi);
}

static void window_chat_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	//Textbox always uses the colour 1 for the text.
	//Translucent flag makes the text invisible.
	if (w->colours[0] & COLOUR_FLAG_TRANSLUCENT) {
		w->colours[1] =  w->colours[0] & ~COLOUR_FLAG_TRANSLUCENT;
	} else {
		w->colours[1] = w->colours[0];
	}

	if (gChatOpen) {
		w->flags &= ~WF_NO_BACKGROUND;

		window_chat_widgets[WIDX_BACKGROUND].type = WWT_FRAME;
		window_chat_widgets[WIDX_TITLE].type = WWT_CAPTION;
		window_chat_widgets[WIDX_RESIZE].type = WWT_RESIZE;
		window_chat_widgets[WIDX_CHAT_INPUT].type = WWT_TEXT_BOX;

		window_chat_widgets[WIDX_CHAT_INPUT].flags = WIDF_TEXT_INPUT_DRAW_LEFT_WRAPPED;
		window_chat_widgets[WIDX_CHAT_HISTORY].flags &= ~WIDF_NO_BACKGROUND & ~WIDF_SCROLLBAR_NO_INTERACT;

		w->scrolls[0].flags |= VSCROLLBAR_VISIBLE;

		window_chat_widgets[WIDX_BACKGROUND].right = w->width - 1;
		window_chat_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

		window_chat_widgets[WIDX_TITLE].right = w->width - 2;

		window_chat_widgets[WIDX_RESIZE].right = w->width - 1;
		window_chat_widgets[WIDX_RESIZE].bottom = w->height - 1;

		char input_buffer[512];
		char* input_string = input_buffer;

		int l = w->x + window_chat_widgets[WIDX_CHAT_INPUT].left;
		int r = w->x + window_chat_widgets[WIDX_CHAT_INPUT].right;

		safe_strcpy(input_buffer, (char*)window_chat_widgets[WIDX_CHAT_INPUT].image, 512);

		int input_height = chat_string_wrapped_get_height((void*)&input_string, r - l - 5 - 6);

		window_chat_widgets[WIDX_CHAT_HISTORY].top = input_height + 18;
		window_chat_widgets[WIDX_CHAT_HISTORY].right = w->width - 3;
		window_chat_widgets[WIDX_CHAT_HISTORY].bottom = w->height - 15;

		window_chat_widgets[WIDX_CHAT_INPUT].right = w->width - 3;
		window_chat_widgets[WIDX_CHAT_INPUT].bottom = 15 + input_height;

		if (!gUsingWidgetTextBox) {
			window_start_textbox(w, WIDX_CHAT_INPUT, 1170, (uint32)_chatCurrentLine, 200);
		}
	} else {
		w->flags |= WF_NO_BACKGROUND;

		window_chat_widgets[WIDX_BACKGROUND].type = WWT_EMPTY;
		window_chat_widgets[WIDX_TITLE].type = WWT_EMPTY;
		window_chat_widgets[WIDX_RESIZE].type = WWT_EMPTY;
		window_chat_widgets[WIDX_CHAT_INPUT].type = WWT_EMPTY;
		window_chat_widgets[WIDX_CHAT_HISTORY].flags = WIDF_NO_BACKGROUND | WIDF_SCROLLBAR_NO_INTERACT;
		window_chat_widgets[WIDX_CHAT_HISTORY].top = 28;
		w->scrolls[0].flags &= ~VSCROLLBAR_VISIBLE;
	}
}

static void window_chat_resize(rct_window *w)
{
	window_set_resize(w, CHAT_MIN_WIDTH, CHAT_MIN_HEIGHT, CHAT_MAX_WIDTH, CHAT_MAX_HEIGHT);
}

static void window_chat_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	char lineBuffer[CHAT_INPUT_SIZE + 10];
	char* lineCh = lineBuffer;

	for (int i = 0; i < CHAT_HISTORY_SIZE; i++) {
		if (strlen(chat_history_get(i)) == 0) 
			continue;

		safe_strcpy(lineBuffer, chat_history_get(i), CHAT_INPUT_SIZE + 10);
		*height += chat_string_wrapped_get_height((void*)&lineCh, w->width - 20) + 5;
	}

	*height += 10;

}

static void window_chat_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	int y = 5;
	char lineBuffer[CHAT_INPUT_SIZE + 10];
	char* lineCh = lineBuffer;

	for (int i = 0; i < CHAT_HISTORY_SIZE; i++) {
		if (y < -50) {
			break;
		}
		
		if (!gChatOpen && SDL_TICKS_PASSED(SDL_GetTicks(), chat_history_get_time(i) + 10000)) {
			break;
		}

		safe_strcpy(lineBuffer, chat_history_get(i), CHAT_INPUT_SIZE + 10);

		y += gfx_draw_string_left_wrapped(dpi, (void*)&lineCh, 0, y, w->width - 20, STR_STRING, COLOUR_WHITE) + 5;
	}
}

static void window_chat_update(rct_window *w)
{
	if (gCurrentTextBox.window.classification == w->classification &&
		gCurrentTextBox.window.number == w->number) {
		window_update_textbox_caret();
	}

	window_invalidate(w);
}

static void window_chat_mouseup(rct_window *w, int widgetIndex)
{
	if (widgetIndex == WIDX_CHAT_INPUT) {
		window_start_textbox(w, widgetIndex, 1170, (uint32)_chatCurrentLine, 200);
	}
}

static void window_chat_textinput(rct_window *w, int widgetIndex, char *text)
{
	if (widgetIndex == WIDX_CHAT_INPUT) {
		if (text != NULL) {
			strcpy(_chatCurrentLine, text);
		}
	}
}