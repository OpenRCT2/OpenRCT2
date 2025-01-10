/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../core/UTF8.h"
#include "../drawing/Drawing.h"
#include "../drawing/Text.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../network/network.h"
#include "../platform/Platform.h"
#include "../world/Location.hpp"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

bool gChatOpen = false;
static u8string _chatCurrentLine;
static std::deque<u8string> _chatHistory;
static std::deque<uint32_t> _chatHistoryTime;
static uint32_t _chatCaretTicks = 0;
static int32_t _chatLeft;
static int32_t _chatTop;
static int32_t _chatRight;
static int32_t _chatBottom;
static int32_t _chatWidth;
static int32_t _chatHeight;
static TextInputSession* _chatTextInputSession;

static const u8string& ChatGetHistory(size_t index);
static uint32_t ChatHistoryGetTime(size_t index);
static void ChatClearInput();
static int32_t ChatHistoryDrawString(DrawPixelInfo& dpi, const char* text, const ScreenCoordsXY& screenCoords, int32_t width);

bool ChatAvailable()
{
    return NetworkGetMode() != NETWORK_MODE_NONE && NetworkGetStatus() == NETWORK_STATUS_CONNECTED
        && NetworkGetAuthstatus() == NetworkAuth::Ok;
}

void ChatOpen()
{
    gChatOpen = true;
    _chatTextInputSession = ContextStartTextInput(_chatCurrentLine, kChatMaxMessageLength);
}

void ChatClose()
{
    gChatOpen = false;
    ContextStopTextInput();
}

void ChatToggle()
{
    if (gChatOpen)
    {
        ChatClose();
    }
    else
    {
        ChatOpen();
    }
}

void ChatInit()
{
    _chatHistory.clear();
    _chatHistoryTime.clear();
}

void ChatUpdate()
{
    // Flash the caret
    _chatCaretTicks = (_chatCaretTicks + 1) % 30;
}

void ChatDraw(DrawPixelInfo& dpi, ColourWithFlags chatBackgroundColor)
{
    thread_local std::string lineBuffer;

    if (!ChatAvailable())
    {
        gChatOpen = false;
        return;
    }

    _chatLeft = 10;
    _chatRight = std::min<int16_t>((ContextGetWidth() - 10), kChatMaxWindowWidth);
    _chatWidth = _chatRight - _chatLeft;
    _chatBottom = ContextGetHeight() - 45;
    _chatTop = _chatBottom - 10;

    const char* inputLine = _chatCurrentLine.c_str();
    int32_t inputLineHeight = 10;

    // Draw chat window
    if (gChatOpen)
    {
        inputLineHeight = ChatStringWrappedGetHeight(inputLine, _chatWidth - 10);
        _chatTop -= inputLineHeight;

        for (const auto& entry : _chatHistory)
        {
            if (entry.empty())
            {
                continue;
            }
            lineBuffer = entry;
            int32_t lineHeight = ChatStringWrappedGetHeight(lineBuffer, _chatWidth - 10);
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
        GfxSetDirtyBlocks(
            { topLeft - ScreenCoordsXY{ 0, 5 }, bottomRight + ScreenCoordsXY{ 0, 5 } }); // Background area + Textbox
        GfxFilterRect(
            dpi, { topLeft - ScreenCoordsXY{ 0, 5 }, bottomRight + ScreenCoordsXY{ 0, 5 } },
            FilterPaletteID::Palette51); // Opaque grey background
        GfxFillRectInset(
            dpi, { topLeft - ScreenCoordsXY{ 0, 5 }, bottomRight + ScreenCoordsXY{ 0, 5 } }, chatBackgroundColor,
            INSET_RECT_FLAG_FILL_NONE);
        GfxFillRectInset(
            dpi, { topLeft + ScreenCoordsXY{ 1, -4 }, bottomRight - ScreenCoordsXY{ 1, inputLineHeight + 6 } },
            chatBackgroundColor, INSET_RECT_FLAG_BORDER_INSET);
        GfxFillRectInset(
            dpi, { bottomLeft + ScreenCoordsXY{ 1, -inputLineHeight - 5 }, bottomRight + ScreenCoordsXY{ -1, 4 } },
            chatBackgroundColor,
            INSET_RECT_FLAG_BORDER_INSET); // Textbox
    }

    auto screenCoords = ScreenCoordsXY{ _chatLeft + 5, _chatBottom - inputLineHeight - 20 };
    int32_t stringHeight = 0;

    // Draw chat history
    for (size_t i = 0; i < kChatHistorySize; i++, screenCoords.y -= stringHeight)
    {
        if (i >= _chatHistory.size())
            break;

        uint32_t expireTime = ChatHistoryGetTime(i) + 10000;
        if (!gChatOpen && Platform::GetTicks() > expireTime)
        {
            break;
        }

        lineBuffer = ChatGetHistory(i);
        auto lineCh = lineBuffer.c_str();
        stringHeight = ChatHistoryDrawString(dpi, lineCh, screenCoords, _chatWidth - 10) + 5;
        GfxSetDirtyBlocks(
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
            dpi, screenCoords + ScreenCoordsXY{ 0, 3 }, _chatWidth - 10, STR_STRING, ft, { kTextColour255 });
        GfxSetDirtyBlocks({ screenCoords, { screenCoords + ScreenCoordsXY{ _chatWidth, inputLineHeight + 15 } } });

        // TODO: Show caret if the input text has multiple lines
        if (_chatCaretTicks < 15 && GfxGetStringWidth(lineBuffer, FontStyle::Medium) < (_chatWidth - 10))
        {
            lineBuffer.assign(_chatCurrentLine.c_str(), _chatTextInputSession->SelectionStart);
            int32_t caretX = screenCoords.x + GfxGetStringWidth(lineBuffer, FontStyle::Medium);
            int32_t caretY = screenCoords.y + 14;

            GfxFillRect(dpi, { { caretX, caretY }, { caretX + 6, caretY + 1 } }, PALETTE_INDEX_56);
        }
    }
}

/**
 * strftime wrapper which appends to an existing string.
 */
static size_t StrCatFTime(char* buffer, size_t bufferSize, const char* format, const struct tm* tp)
{
    size_t stringLen = strnlen(buffer, bufferSize);
    if (stringLen < bufferSize)
    {
        char* dst = buffer + stringLen;
        size_t dstMaxSize = bufferSize - stringLen;
        return strftime(dst, dstMaxSize, format, tp);
    }
    return 0;
}

void ChatAddHistory(std::string_view s)
{
    // Format a timestamp
    time_t timer{};
    time(&timer);
    auto tmInfo = localtime(&timer);
    char timeBuffer[64]{};
    StrCatFTime(timeBuffer, sizeof(timeBuffer), "[%H:%M] ", tmInfo);

    std::string buffer = timeBuffer;
    buffer += s;

    if (_chatHistory.size() >= kChatHistorySize)
    {
        _chatHistory.pop_back();
        _chatHistoryTime.pop_back();
    }

    _chatHistory.push_front(buffer);
    _chatHistoryTime.push_front(Platform::GetTicks());

    // Log to file (src only as logging does its own timestamp)
    NetworkAppendChatLog(s);

    CreateAudioChannel(SoundId::NewsItem, 0, kMixerVolumeMax, 0.5f, 1.5f, true);
}

void ChatInput(enum ChatInput input)
{
    switch (input)
    {
        case ChatInput::Send:
            if (!_chatCurrentLine.empty())
            {
                NetworkSendChat(_chatCurrentLine.c_str());
            }
            ChatClearInput();
            ChatClose();
            break;
        case ChatInput::Close:
            ChatClose();
            break;
        default:
            break;
    }
}

static const u8string& ChatGetHistory(size_t index)
{
    return _chatHistory[index];
}

static uint32_t ChatHistoryGetTime(size_t index)
{
    return _chatHistoryTime[index];
}

static void ChatClearInput()
{
    _chatCurrentLine.clear();
}

// This method is the same as gfx_draw_string_left_wrapped.
// But this adjusts the initial Y coordinate depending of the number of lines.
static int32_t ChatHistoryDrawString(DrawPixelInfo& dpi, const char* text, const ScreenCoordsXY& screenCoords, int32_t width)
{
    int32_t numLines;
    u8string wrappedString;
    GfxWrapString(FormatString("{OUTLINE}{WHITE}{STRING}", text), width, FontStyle::Medium, &wrappedString, &numLines);
    auto lineHeight = FontGetLineHeight(FontStyle::Medium);

    int32_t expectedY = screenCoords.y - (numLines * lineHeight);
    if (expectedY < 50)
    {
        return (numLines * lineHeight); // Skip drawing, return total height.
    }

    const utf8* bufferPtr = wrappedString.data();
    int32_t lineY = screenCoords.y;
    for (int32_t line = 0; line <= numLines; ++line)
    {
        DrawText(dpi, { screenCoords.x, lineY - (numLines * lineHeight) }, { kTextColour254 }, bufferPtr);
        bufferPtr = GetStringEnd(bufferPtr) + 1;
        lineY += lineHeight;
    }
    return lineY - screenCoords.y;
}

// Wrap string without drawing, useful to get the height of a wrapped string.
// Almost the same as gfx_draw_string_left_wrapped
int32_t ChatStringWrappedGetHeight(u8string_view args, int32_t width)
{
    int32_t numLines;
    GfxWrapString(FormatStringID(STR_STRING, args), width, FontStyle::Medium, nullptr, &numLines);
    const int32_t lineHeight = FontGetLineHeight(FontStyle::Medium);
    return lineHeight * (numLines + 1);
}
