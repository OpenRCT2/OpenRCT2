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

#define CHAT_HISTORY_SIZE 10
#define CHAT_INPUT_SIZE 256

bool gChatOpen = false;
char _chatCurrentLine[CHAT_INPUT_SIZE];
char _chatHistory[CHAT_HISTORY_SIZE][CHAT_INPUT_SIZE];
uint32 _chatHistoryTime[CHAT_HISTORY_SIZE];
unsigned int _chatHistoryIndex = 0;
uint32 _chatCaretTicks = 0;
int _chatLeft;
int _chatTop;
int _chatRight;
int _chatBottom;

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

void chat_draw()
{
	if (network_get_mode() == NETWORK_MODE_NONE || network_get_status() != NETWORK_STATUS_CONNECTED || network_get_authstatus() != NETWORK_AUTH_OK) {
		gChatOpen = false;
		return;
	}
	rct_drawpixelinfo *dpi = &gScreenDPI;
	_chatLeft = 10;
	_chatTop = gScreenHeight - 40 - ((CHAT_HISTORY_SIZE + 1) * 10);
	_chatRight = gScreenWidth - 10;
	_chatBottom = gScreenHeight - 45;
	char lineBuffer[CHAT_INPUT_SIZE + 10];
	char* lineCh = lineBuffer;
	int x = _chatLeft;
	int y = _chatBottom - (15 * 2);
	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	gCurrentFontFlags = 0;
	for (int i = 0; i < CHAT_HISTORY_SIZE; i++, y -= 15) {
		if (!gChatOpen && SDL_TICKS_PASSED(SDL_GetTicks(), chat_history_get_time(i) + 10000)) {
			break;
		}
		safe_strcpy(lineBuffer, chat_history_get(i), CHAT_INPUT_SIZE + 10);
		gfx_set_dirty_blocks(x, y, x + gfx_get_string_width(lineBuffer), y + 12);
		gfx_draw_string(dpi, lineBuffer, 255, x, y);
	}
	if (gChatOpen) {
		lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
		lineCh = utf8_write_codepoint(lineCh, FORMAT_CELADON);
		safe_strcpy(lineCh, _chatCurrentLine, CHAT_INPUT_SIZE);
		y = _chatBottom - 15;
		gfx_set_dirty_blocks(x, y, x + gfx_get_string_width(lineBuffer) + 7, y + 12);
		gfx_draw_string(dpi, lineBuffer, 255, x, y);
		if (_chatCaretTicks < 15) {
			memcpy(lineBuffer, _chatCurrentLine, gTextInput.selection_offset);
			lineBuffer[gTextInput.selection_offset] = 0;
			int caretX = x + gfx_get_string_width(lineBuffer);
			int caretY = y + 15;

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
		break;
	case SDL_SCANCODE_ESCAPE:
		chat_close();
		break;
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
