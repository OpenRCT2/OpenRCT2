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
#include "../core/String.hpp"

#include <string_view>

#define CHAT_HISTORY_SIZE 10
#define CHAT_INPUT_SIZE 1024
#define CHAT_MAX_MESSAGE_LENGTH 200
#define CHAT_MAX_WINDOW_WIDTH 600

struct DrawPixelInfo;
struct ScreenCoordsXY;

enum class ChatInput : uint8_t
{
    None,
    Send,
    Close,
};

extern bool gChatOpen;

bool ChatAvailable();
void ChatOpen();
void ChatClose();
void ChatToggle();

void ChatInit();
void ChatUpdate();
void ChatDraw(DrawPixelInfo* dpi, uint8_t chatBackgroundColour);

void ChatAddHistory(std::string_view s);
void ChatInput(ChatInput input);

int32_t ChatStringWrappedGetHeight(u8string_view args, int32_t width);
