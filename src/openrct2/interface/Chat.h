/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"
#include "Colour.h"

#include <string_view>

constexpr int8_t kChatHistorySize = 10;
constexpr int16_t kChatInputSize = 1024;
constexpr uint8_t kChatMaxMessageLength = 200;
constexpr int16_t kChatMaxWindowWidth = 600;

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
void ChatDraw(DrawPixelInfo& dpi, ColourWithFlags chatBackgroundColour);

void ChatAddHistory(std::string_view s);
void ChatInput(ChatInput input);

int32_t ChatStringWrappedGetHeight(u8string_view args, int32_t width);
