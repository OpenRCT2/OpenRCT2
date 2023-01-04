/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <string_view>

#define CHAT_HISTORY_SIZE 10
#define CHAT_INPUT_SIZE 1024
#define CHAT_MAX_MESSAGE_LENGTH 200
#define CHAT_MAX_WINDOW_WIDTH 600

struct rct_drawpixelinfo;
struct ScreenCoordsXY;

enum class ChatInput : uint8_t
{
    None,
    Send,
    Close,
};

extern bool gChatOpen;

bool chat_available();
void chat_open();
void chat_close();
void chat_toggle();

void chat_init();
void chat_update();
void chat_draw(rct_drawpixelinfo* dpi, uint8_t chatBackgroundColour);

void chat_history_add(std::string_view s);
void chat_input(ChatInput input);

int32_t chat_string_wrapped_get_height(void* args, int32_t width);
