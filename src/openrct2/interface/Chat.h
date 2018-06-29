/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _CHAT_H_
#define _CHAT_H_

#include "../common.h"

#define CHAT_HISTORY_SIZE 10
#define CHAT_INPUT_SIZE 1024
#define CHAT_MAX_MESSAGE_LENGTH 200
#define CHAT_MAX_WINDOW_WIDTH 600

struct rct_drawpixelinfo;

enum CHAT_INPUT
{
    CHAT_INPUT_NONE,
    CHAT_INPUT_SEND,
    CHAT_INPUT_CLOSE,
};

extern bool gChatOpen;

void chat_open();
void chat_close();
void chat_toggle();

void chat_init();
void chat_update();
void chat_draw(rct_drawpixelinfo * dpi, uint8_t chatBackgroundColour);

void chat_history_add(const char *src);
void chat_input(CHAT_INPUT input);

int32_t chat_string_wrapped_get_height(void *args, int32_t width);
int32_t chat_history_draw_string(rct_drawpixelinfo *dpi, void *args, int32_t x, int32_t y, int32_t width);

#endif
