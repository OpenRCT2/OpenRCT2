/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Chat.h"

#include "../Context.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../drawing/Drawing.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../platform/Platform.h"
#include "../util/Util.h"
#include "../world/Location.hpp"

#include <algorithm>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

bool gChatOpen = false;
static char _chatCurrentLine[CHAT_MAX_MESSAGE_LENGTH];
static char _chatHistory[CHAT_HISTORY_SIZE][CHAT_INPUT_SIZE];
static uint32_t _chatHistoryTime[CHAT_HISTORY_SIZE];
static uint32_t _chatHistoryIndex = 0;
static uint32_t _chatCaretTicks = 0;
static int32_t _chatLeft;
static int32_t _chatTop;
static int32_t _chatRight;
static int32_t _chatBottom;
static int32_t _chatWidth;
static int32_t _chatHeight;
static TextInputSession* _chatTextInputSession;

static const char* chat_history_get(uint32_t index);
static uint32_t chat_history_get_time(uint32_t index);
static void chat_clear_input();
static int32_t chat_history_draw_string(
    rct_drawpixelinfo* dpi, const char* text, const ScreenCoordsXY& screenCoords, int32_t width);

bool chat_available()
{
    return network_get_mode() != NETWORK_MODE_NONE && network_get_status() == NETWORK_STATUS_CONNECTED
        && network_get_authstatus() == NetworkAuth::Ok;
}

void chat_open()
{
    gChatOpen = true;
    _chatTextInputSession = ContextStartTextInput(_chatCurrentLine, sizeof(_chatCurrentLine));
}

void chat_close()
{
    gChatOpen = false;
    ContextStopTextInput();
}

void chat_toggle()
{
    if (gChatOpen)
    {
        chat_close();
    }
    else
    {
        chat_open();
    }
}

void chat_init()
{
    std::memset(_chatHistory, 0x00, sizeof(_chatHistory));
    std::memset(_chatHistoryTime, 0x00, sizeof(_chatHistoryTime));
}

void chat_update()
{
    // Flash the caret
    _chatCaretTicks = (_chatCaretTicks + 1) % 30;
}

void chat_draw(rct_drawpixelinfo* dpi, uint8_t chatBackgroundColor)
{
    thread_local std::string lineBuffer;

    if (!chat_available())
    {
        gChatOpen = false;
        return;
    }

    _chatLeft = 10;
    _chatRight = std::min((ContextGetWidth() - 10), CHAT_MAX_WINDOW_WIDTH);
    _chatWidth = _chatRight - _chatLeft;
    _chatBottom = ContextGetHeight() - 45;
    _chatTop = _chatBottom - 10;

    char* inputLine = _chatCurrentLine;
    int32_t inputLineHeight = 10;

    // Draw chat window
    if (gChatOpen)
    {
        inputLineHeight = chat_string_wrapped_get_height(static_cast<void*>(&inputLine), _chatWidth - 10);
        _chatTop -= inputLineHeight;

        for (int32_t i = 0; i < CHAT_HISTORY_SIZE; i++)
        {
            if (chat_history_get(i)[0] == '\0')
            {
                continue;
            }

            lineBuffer.assign(chat_history_get(i));
            auto lineCh = lineBuffer.c_str();
            int32_t lineHeight = chat_string_wrapped_get_height(static_cast<void*>(&lineCh), _chatWidth - 10);
            _chatTop -= (lineHeight + 5);
        }

        _chatHeight = _chatBottom - _chatTop;

        if (_chatTop < 50)
        {
            _chatTop = 50;
        }
        else if (_chatHeight < 150)
        { // Min height
            _chatTop = _chatBottom - 150;
            _chatHeight = 150;
        }

        ScreenCoordsXY topLeft{ _chatLeft, _chatTop };
        ScreenCoordsXY bottomRight{ _chatRight, _chatBottom };
        ScreenCoordsXY bottomLeft{ _chatLeft, _chatBottom };
        gfx_set_dirty_blocks(
            { topLeft - ScreenCoordsXY{ 0, 5 }, bottomRight + ScreenCoordsXY{ 0, 5 } }); // Background area + Textbox
        gfx_filter_rect(
            dpi, { topLeft - ScreenCoordsXY{ 0, 5 }, bottomRight + ScreenCoordsXY{ 0, 5 } },
            FilterPaletteID::Palette51); // Opaque grey background
        gfx_fill_rect_inset(
            dpi, { topLeft - ScreenCoordsXY{ 0, 5 }, bottomRight + ScreenCoordsXY{ 0, 5 } }, chatBackgroundColor,
            INSET_RECT_FLAG_FILL_NONE);
        gfx_fill_rect_inset(
            dpi, { topLeft + ScreenCoordsXY{ 1, -4 }, bottomRight - ScreenCoordsXY{ 1, inputLineHeight + 6 } },
            chatBackgroundColor, INSET_RECT_FLAG_BORDER_INSET);
        gfx_fill_rect_inset(
            dpi, { bottomLeft + ScreenCoordsXY{ 1, -inputLineHeight - 5 }, bottomRight + ScreenCoordsXY{ -1, 4 } },
            chatBackgroundColor,
            INSET_RECT_FLAG_BORDER_INSET); // Textbox
    }

    auto screenCoords = ScreenCoordsXY{ _chatLeft + 5, _chatBottom - inputLineHeight - 20 };
    int32_t stringHeight = 0;

    // Draw chat history
    for (int32_t i = 0; i < CHAT_HISTORY_SIZE; i++, screenCoords.y -= stringHeight)
    {
        uint32_t expireTime = chat_history_get_time(i) + 10000;
        if (!gChatOpen && Platform::GetTicks() > expireTime)
        {
            break;
        }

        lineBuffer.assign(chat_history_get(i));
        auto lineCh = lineBuffer.c_str();
        stringHeight = chat_history_draw_string(dpi, lineCh, screenCoords, _chatWidth - 10) + 5;
        gfx_set_dirty_blocks(
            { { screenCoords - ScreenCoordsXY{ 0, stringHeight } }, { screenCoords + ScreenCoordsXY{ _chatWidth, 20 } } });

        if ((screenCoords.y - stringHeight) < 50)
        {
            break;
        }
    }

    // Draw current chat input
    if (gChatOpen)
    {
        lineBuffer.assign("{OUTLINE}{CELADON}");
        lineBuffer += _chatCurrentLine;

        screenCoords.y = _chatBottom - inputLineHeight - 5;

        auto lineCh = lineBuffer.c_str();
        auto ft = Formatter();
        ft.Add<const char*>(lineCh);
        inputLineHeight = DrawTextWrapped(
            dpi, screenCoords + ScreenCoordsXY{ 0, 3 }, _chatWidth - 10, STR_STRING, ft, { TEXT_COLOUR_255 });
        gfx_set_dirty_blocks({ screenCoords, { screenCoords + ScreenCoordsXY{ _chatWidth, inputLineHeight + 15 } } });

        // TODO: Show caret if the input text has multiple lines
        if (_chatCaretTicks < 15 && gfx_get_string_width(lineBuffer, FontStyle::Medium) < (_chatWidth - 10))
        {
            lineBuffer.assign(_chatCurrentLine, _chatTextInputSession->SelectionStart);
            int32_t caretX = screenCoords.x + gfx_get_string_width(lineBuffer, FontStyle::Medium);
            int32_t caretY = screenCoords.y + 14;

            gfx_fill_rect(dpi, { { caretX, caretY }, { caretX + 6, caretY + 1 } }, PALETTE_INDEX_56);
        }
    }
}

void chat_history_add(std::string_view s)
{
    // Format a timestamp
    time_t timer{};
    time(&timer);
    auto tmInfo = localtime(&timer);
    char timeBuffer[64]{};
    strcatftime(timeBuffer, sizeof(timeBuffer), "[%H:%M] ", tmInfo);

    std::string buffer = timeBuffer;
    buffer += s;

    // Add to history list
    int32_t index = _chatHistoryIndex % CHAT_HISTORY_SIZE;
    std::fill_n(_chatHistory[index], CHAT_INPUT_SIZE, 0x00);
    std::memcpy(_chatHistory[index], buffer.c_str(), std::min<size_t>(buffer.size(), CHAT_INPUT_SIZE - 1));
    _chatHistoryTime[index] = Platform::GetTicks();
    _chatHistoryIndex++;

    // Log to file (src only as logging does its own timestamp)
    network_append_chat_log(s);

    CreateAudioChannel(SoundId::NewsItem, 0, MIXER_VOLUME_MAX, 0.5f, 1.5f, true);
}

void chat_input(ChatInput input)
{
    switch (input)
    {
        case ChatInput::Send:
            if (_chatCurrentLine[0] != '\0')
            {
                network_send_chat(_chatCurrentLine);
            }
            chat_clear_input();
            chat_close();
            break;
        case ChatInput::Close:
            chat_close();
            break;
        default:
            break;
    }
}

static const char* chat_history_get(uint32_t index)
{
    return _chatHistory[(_chatHistoryIndex + CHAT_HISTORY_SIZE - index - 1) % CHAT_HISTORY_SIZE];
}

static uint32_t chat_history_get_time(uint32_t index)
{
    return _chatHistoryTime[(_chatHistoryIndex + CHAT_HISTORY_SIZE - index - 1) % CHAT_HISTORY_SIZE];
}

static void chat_clear_input()
{
    _chatCurrentLine[0] = 0;
}

// This method is the same as gfx_draw_string_left_wrapped.
// But this adjusts the initial Y coordinate depending of the number of lines.
static int32_t chat_history_draw_string(
    rct_drawpixelinfo* dpi, const char* text, const ScreenCoordsXY& screenCoords, int32_t width)
{
    char buffer[CommonTextBufferSize];
    auto bufferPtr = buffer;
    FormatStringToBuffer(buffer, sizeof(buffer), "{OUTLINE}{WHITE}{STRING}", text);

    int32_t numLines;
    gfx_wrap_string(bufferPtr, width, FontStyle::Medium, &numLines);
    auto lineHeight = font_get_line_height(FontStyle::Medium);

    int32_t expectedY = screenCoords.y - (numLines * lineHeight);
    if (expectedY < 50)
    {
        return (numLines * lineHeight); // Skip drawing, return total height.
    }

    auto lineY = screenCoords.y;
    for (int32_t line = 0; line <= numLines; ++line)
    {
        gfx_draw_string(dpi, { screenCoords.x, lineY - (numLines * lineHeight) }, bufferPtr, { TEXT_COLOUR_254 });
        bufferPtr = get_string_end(bufferPtr) + 1;
        lineY += lineHeight;
    }
    return lineY - screenCoords.y;
}

// Wrap string without drawing, useful to get the height of a wrapped string.
// Almost the same as gfx_draw_string_left_wrapped
int32_t chat_string_wrapped_get_height(void* args, int32_t width)
{
    char buffer[CommonTextBufferSize];
    auto bufferPtr = buffer;
    format_string(bufferPtr, 256, STR_STRING, args);

    int32_t numLines;
    gfx_wrap_string(bufferPtr, width, FontStyle::Medium, &numLines);
    int32_t lineHeight = font_get_line_height(FontStyle::Medium);

    int32_t lineY = 0;
    for (int32_t line = 0; line <= numLines; ++line)
    {
        bufferPtr = get_string_end(bufferPtr) + 1;
        lineY += lineHeight;
    }

    return lineY;
}
