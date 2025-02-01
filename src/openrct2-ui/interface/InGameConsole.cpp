/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "InGameConsole.h"

#include "../UiStringIds.h"
#include "Theme.h"

#include <algorithm>
#include <cstring>
#include <openrct2/Context.h>
#include <openrct2/Version.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/UTF8.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/interface/Window.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/LocalisationService.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

static InGameConsole _inGameConsole;

static FontStyle InGameConsoleGetFontStyle()
{
    return (Config::Get().interface.ConsoleSmallFont ? FontStyle::Small : FontStyle::Medium);
}

static int32_t InGameConsoleGetLineHeight()
{
    return FontGetLineHeight(InGameConsoleGetFontStyle());
}

void InGameConsole::WriteInitial()
{
    InteractiveConsole::WriteLine(OPENRCT2_NAME " " kOpenRCT2Version);
    InteractiveConsole::WriteLine(LanguageGetString(STR_CONSOLE_HELPER_TEXT));
    InteractiveConsole::WriteLine("");
    WritePrompt();
}

void InGameConsole::WritePrompt()
{
    InteractiveConsole::WriteLine("> ");
}

void InGameConsole::Input(ConsoleInput input)
{
    if (_isCommandAwaitingCompletion)
    {
        // Do not process input while a command is running
        return;
    }

    switch (input)
    {
        case ConsoleInput::LineClear:
            ClearInput();
            RefreshCaret();
            break;
        case ConsoleInput::LineExecute:
            if (_consoleCurrentLine[0] != '\0')
            {
                HistoryAdd(_consoleCurrentLine);

                // Append text we are executing to prompt line
                _consoleLines.back().first.append(_consoleCurrentLine);

                Execute(_consoleCurrentLine);
                if (IsExecuting())
                {
                    _isCommandAwaitingCompletion = true;
                }
                else
                {
                    WritePrompt();
                }
                ClearInput();
                RefreshCaret();
            }
            ScrollToEnd();
            break;
        case ConsoleInput::HistoryPrevious:
            if (_consoleHistoryIndex > 0)
            {
                _consoleHistoryIndex--;
                _consoleCurrentLine = _consoleHistory[_consoleHistoryIndex];
            }
            _consoleTextInputSession->Length = UTF8Length(_consoleCurrentLine.c_str());
            _consoleTextInputSession->SelectionStart = _consoleCurrentLine.size();
            RefreshCaret(_consoleTextInputSession->SelectionStart);
            break;
        case ConsoleInput::HistoryNext:
            if (_consoleHistoryIndex + 1 < _consoleHistory.size())
            {
                _consoleHistoryIndex++;
                _consoleCurrentLine = _consoleHistory[_consoleHistoryIndex];
                _consoleTextInputSession->Length = UTF8Length(_consoleCurrentLine.c_str());
                _consoleTextInputSession->SelectionStart = _consoleCurrentLine.size();
            }
            else
            {
                _consoleHistoryIndex = _consoleHistory.size();
                ClearInput();
            }
            RefreshCaret(_consoleTextInputSession->SelectionStart);
            break;
        case ConsoleInput::ScrollPrevious:
        {
            int32_t scrollAmt = GetNumVisibleLines() - 1;
            Scroll(scrollAmt);
            break;
        }
        case ConsoleInput::ScrollNext:
        {
            int32_t scrollAmt = GetNumVisibleLines() - 1;
            Scroll(-scrollAmt);
            break;
        }
        default:
            break;
    }
}

void InGameConsole::ClearInput()
{
    _consoleCurrentLine.clear();
    if (_isOpen)
    {
        _consoleTextInputSession = ContextStartTextInput(_consoleCurrentLine, kConsoleInputSize);
    }
}

void InGameConsole::HistoryAdd(const u8string& src)
{
    if (_consoleHistory.size() >= kConsoleHistorySize)
    {
        _consoleHistory.pop_front();
    }
    _consoleHistory.push_back(src);
    _consoleHistoryIndex = _consoleHistory.size();
}

void InGameConsole::ScrollToEnd()
{
    const int32_t maxLines = GetNumVisibleLines();
    if (maxLines == 0)
        _consoleScrollPos = 0;
    else
        _consoleScrollPos = std::max<int32_t>(0, static_cast<int32_t>(_consoleLines.size()) - maxLines);
}

void InGameConsole::RefreshCaret(size_t position)
{
    _consoleCaretTicks = 0;
    _selectionStart = position;

    auto text = u8string_view{ _consoleCurrentLine }.substr(0, _selectionStart);
    _caretScreenPosX = GfxGetStringWidthNoFormatting(text, InGameConsoleGetFontStyle());
}

void InGameConsole::Scroll(int32_t linesToScroll)
{
    const int32_t maxVisibleLines = GetNumVisibleLines();
    const int32_t numLines = static_cast<int32_t>(_consoleLines.size());
    if (numLines > maxVisibleLines)
    {
        int32_t maxScrollValue = numLines - maxVisibleLines;
        _consoleScrollPos = std::clamp<int32_t>(_consoleScrollPos - linesToScroll, 0, maxScrollValue);
    }
}

void InGameConsole::Clear()
{
    _consoleLines.clear();
    ScrollToEnd();
}

void InGameConsole::ClearLine()
{
    _consoleCurrentLine[0] = 0;
    RefreshCaret();
}

void InGameConsole::Open()
{
    if (!_isInitialised)
    {
        WriteInitial();
        _isInitialised = true;
    }

    _isOpen = true;
    ScrollToEnd();
    RefreshCaret();
    _consoleTextInputSession = ContextStartTextInput(_consoleCurrentLine, kConsoleInputSize);
}

void InGameConsole::Close()
{
    _consoleTextInputSession = nullptr;
    _isOpen = false;
    Invalidate();
    ContextStopTextInput();
}

void InGameConsole::Hide()
{
    Close();
}

void InGameConsole::Toggle()
{
    if (_isOpen)
    {
        Close();
    }
    else
    {
        Open();
    }
}

void InGameConsole::WriteLine(const std::string& input, FormatToken colourFormat)
{
    std::string line;
    std::size_t splitPos = 0;
    std::size_t stringOffset = 0;
    while (splitPos != std::string::npos)
    {
        splitPos = input.find('\n', stringOffset);
        line = input.substr(stringOffset, splitPos - stringOffset);
        _consoleLines.emplace_back(line, colourFormat);
        stringOffset = splitPos + 1;
    }

    if (_consoleLines.size() > kConsoleMaxLines)
    {
        const std::size_t linesToErase = _consoleLines.size() - kConsoleMaxLines;
        _consoleLines.erase(_consoleLines.begin(), _consoleLines.begin() + linesToErase);
    }
}

void InGameConsole::Invalidate() const
{
    GfxSetDirtyBlocks({ _consoleTopLeft, _consoleBottomRight });
}

void InGameConsole::Update()
{
    _consoleTopLeft = { 0, 0 };
    _consoleBottomRight = { ContextGetWidth(), 322 };

    if (_isOpen)
    {
        // When scrolling the map, the console pixels get copied... therefore invalidate the screen
        WindowBase* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            Viewport* mainViewport = WindowGetViewport(mainWindow);
            if (mainViewport != nullptr)
            {
                if (_lastMainViewport != mainViewport->viewPos)
                {
                    _lastMainViewport = mainViewport->viewPos;

                    GfxInvalidateScreen();
                }
            }
        }
    }

    if (_isCommandAwaitingCompletion && !IsExecuting())
    {
        WritePrompt();
        _isCommandAwaitingCompletion = false;
    }

    // Flash the caret
    _consoleCaretTicks = (_consoleCaretTicks + 1) % 30;
}

void InGameConsole::Draw(DrawPixelInfo& dpi) const
{
    if (!_isOpen)
        return;

    // Set font
    ColourWithFlags textColour = { ThemeGetColour(WindowClass::Console, 1).colour, 0 };
    const FontStyle style = InGameConsoleGetFontStyle();
    const int32_t lineHeight = InGameConsoleGetLineHeight();
    const int32_t maxLines = GetNumVisibleLines();

    // TTF looks far better without the outlines
    if (!LocalisationService_UseTrueTypeFont())
    {
        textColour.setFlag(ColourFlag::withOutline, true);
    }

    Invalidate();

    // Give console area a translucent effect.
    GfxFilterRect(dpi, { _consoleTopLeft, _consoleBottomRight }, FilterPaletteID::Palette51);

    // Make input area more opaque.
    GfxFilterRect(
        dpi, { { _consoleTopLeft.x, _consoleBottomRight.y - lineHeight - 10 }, _consoleBottomRight - ScreenCoordsXY{ 0, 1 } },
        FilterPaletteID::Palette51);

    // Paint background colour.
    auto backgroundColour = ThemeGetColour(WindowClass::Console, 0);
    GfxFillRectInset(dpi, { _consoleTopLeft, _consoleBottomRight }, backgroundColour, INSET_RECT_FLAG_FILL_NONE);
    GfxFillRectInset(
        dpi, { _consoleTopLeft + ScreenCoordsXY{ 1, 1 }, _consoleBottomRight - ScreenCoordsXY{ 1, 1 } }, backgroundColour,
        INSET_RECT_FLAG_BORDER_INSET);

    std::string lineBuffer;
    auto screenCoords = _consoleTopLeft + ScreenCoordsXY{ kConsoleEdgePadding, kConsoleEdgePadding };

    // Draw text inside console
    for (std::size_t i = 0; i < _consoleLines.size() && i < static_cast<size_t>(maxLines); i++)
    {
        const size_t index = i + _consoleScrollPos;
        if (_consoleLines[index].second == FormatToken::ColourWindow2)
        {
            // This is something of a hack to ensure the text is actually black
            // as opposed to a desaturated grey
            if (textColour.colour == COLOUR_BLACK)
            {
                DrawText(dpi, screenCoords, { textColour, style }, "{BLACK}");
                DrawText(dpi, screenCoords, { kTextColour255, style }, _consoleLines[index].first.c_str(), true);
            }
            else
            {
                DrawText(dpi, screenCoords, { textColour, style }, _consoleLines[index].first.c_str(), true);
            }
        }
        else
        {
            std::string lineColour = FormatTokenToStringWithBraces(_consoleLines[index].second);
            DrawText(dpi, screenCoords, { textColour, style }, lineColour.c_str());
            DrawText(dpi, screenCoords, { kTextColour255, style }, _consoleLines[index].first.c_str(), true);
        }

        screenCoords.y += lineHeight;
    }

    screenCoords.y = _consoleBottomRight.y - lineHeight - kConsoleEdgePadding - 1;

    // Draw current line
    if (textColour.colour == COLOUR_BLACK)
    {
        DrawText(dpi, screenCoords, { textColour, style }, "{BLACK}");
        DrawText(dpi, screenCoords, { kTextColour255, style }, _consoleCurrentLine.c_str(), true);
    }
    else
    {
        DrawText(dpi, screenCoords, { textColour, style }, _consoleCurrentLine.c_str(), true);
    }

    // Draw caret
    if (_consoleCaretTicks < kConsoleCaretFlashThreshold)
    {
        auto caret = screenCoords + ScreenCoordsXY{ _caretScreenPosX, lineHeight };
        uint8_t caretColour = ColourMapA[textColour.colour].lightest;
        GfxFillRect(dpi, { caret, caret + ScreenCoordsXY{ kConsoleCaretWidth, 1 } }, caretColour);
    }

    // What about border colours?
    uint8_t borderColour1 = ColourMapA[backgroundColour.colour].light;
    uint8_t borderColour2 = ColourMapA[backgroundColour.colour].mid_dark;

    // Input area top border
    GfxFillRect(
        dpi,
        { { _consoleTopLeft.x, _consoleBottomRight.y - lineHeight - 11 },
          { _consoleBottomRight.x, _consoleBottomRight.y - lineHeight - 11 } },
        borderColour1);
    GfxFillRect(
        dpi,
        { { _consoleTopLeft.x, _consoleBottomRight.y - lineHeight - 10 },
          { _consoleBottomRight.x, _consoleBottomRight.y - lineHeight - 10 } },
        borderColour2);

    // Input area bottom border
    GfxFillRect(
        dpi, { { _consoleTopLeft.x, _consoleBottomRight.y - 1 }, { _consoleBottomRight.x, _consoleBottomRight.y - 1 } },
        borderColour1);
    GfxFillRect(dpi, { { _consoleTopLeft.x, _consoleBottomRight.y }, _consoleBottomRight }, borderColour2);
}

// Calculates the amount of visible lines, based on the console size, excluding the input line.
int32_t InGameConsole::GetNumVisibleLines() const
{
    const int32_t lineHeight = InGameConsoleGetLineHeight();
    const int32_t consoleHeight = _consoleBottomRight.y - _consoleTopLeft.y;
    if (consoleHeight == 0)
        return 0;
    const int32_t drawableHeight = consoleHeight - 2 * lineHeight - 4; // input line, separator - padding
    return drawableHeight / lineHeight;
}
