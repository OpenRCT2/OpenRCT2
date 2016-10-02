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

#include "chat.h"
#include "../audio/audio.h"
#include "../audio/mixer.h"
#include "../interface/themes.h"
#include "../localisation/localisation.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../util/util.h"

bool gChatOpen = false;
char _chatCurrentLine[CHAT_MAX_MESSAGE_LENGTH];
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

void chat_open()
{
	gChatOpen = true;
	platform_start_text_input(_chatCurrentLine, sizeof(_chatCurrentLine));
}

void chat_close()
{
	gChatOpen = false;
	platform_stop_text_input();
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

	_chatLeft = 10;
	_chatRight = min((gScreenWidth - 10), CHAT_MAX_WINDOW_WIDTH);
	_chatWidth = _chatRight - _chatLeft;
	_chatBottom = gScreenHeight - 45;
	_chatTop = _chatBottom - 10;

	char lineBuffer[CHAT_INPUT_SIZE + 10];
	char* lineCh = lineBuffer;
	char* inputLine = _chatCurrentLine;
	int inputLineHeight = 10;
	uint8 chatBackgroundColor = theme_get_colour(WC_CHAT, 0);

	// Draw chat window
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
		} else if (_chatHeight < 150) { //Min height
			_chatTop = _chatBottom - 150;
			_chatHeight = 150;
		}

		gfx_set_dirty_blocks(_chatLeft, _chatTop - 5, _chatRight, _chatBottom + 5); //Background area + Textbox
		gfx_fill_rect(dpi, _chatLeft, _chatTop - 5, _chatRight, _chatBottom + 5, 0x2000000 | 51); //Opaque gray background
		gfx_fill_rect_inset(dpi, _chatLeft, _chatTop - 5, _chatRight, _chatBottom + 5, chatBackgroundColor, 0x10);
		gfx_fill_rect_inset(dpi, _chatLeft + 1, _chatTop - 4, _chatRight - 1, _chatBottom - inputLineHeight - 6, chatBackgroundColor, 0x20);
		gfx_fill_rect_inset(dpi, _chatLeft + 1, _chatBottom - inputLineHeight - 5, _chatRight - 1, _chatBottom + 4, chatBackgroundColor, 0x20); //Textbox
	}
	
	int x = _chatLeft + 5;
	int y = _chatBottom - inputLineHeight - 20;
	int stringHeight = 0;

	// Draw chat history
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
	
	// Draw current chat input
	if (gChatOpen) {
		lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
		lineCh = utf8_write_codepoint(lineCh, FORMAT_CELADON);

		safe_strcpy(lineCh, _chatCurrentLine, sizeof(_chatCurrentLine));
		y = _chatBottom - inputLineHeight - 5;

		lineCh = lineBuffer;
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

	gfx_draw_string(dpi, "", 255, dpi->x, dpi->y);
	char *buffer = gCommonStringFormatBuffer;
	format_string(buffer, STR_STRING, args);

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	gfx_wrap_string(buffer, width, &numLines, &fontSpriteBase);
	lineHeight = font_get_line_height(fontSpriteBase);

	gCurrentFontFlags = 0;

	int expectedY = y - (numLines * lineHeight);
	if (expectedY < 50) {
		return (numLines * lineHeight); //Skip drawing, return total height.
	}

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

	char *buffer = gCommonStringFormatBuffer;
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
