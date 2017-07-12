#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#ifndef _CHAT_H_
#define _CHAT_H_

#include "../common.h"
#include "../drawing/drawing.h"

#define CHAT_HISTORY_SIZE 10
#define CHAT_INPUT_SIZE 1024
#define CHAT_MAX_MESSAGE_LENGTH 200
#define CHAT_MAX_WINDOW_WIDTH 600

typedef enum CHAT_INPUT
{
    CHAT_INPUT_NONE,
    CHAT_INPUT_SEND,
    CHAT_INPUT_CLOSE,
} CHAT_INPUT;

extern bool gChatOpen;

void chat_open();
void chat_close();
void chat_toggle();

void chat_init();
void chat_update();
void chat_draw(rct_drawpixelinfo * dpi);

void chat_history_add(const char *src);
void chat_input(CHAT_INPUT input);

sint32 chat_string_wrapped_get_height(void *args, sint32 width);
sint32 chat_history_draw_string(rct_drawpixelinfo *dpi, void *args, sint32 x, sint32 y, sint32 width);

#endif
