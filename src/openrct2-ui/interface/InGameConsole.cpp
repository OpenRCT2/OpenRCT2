/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cstring>
#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/interface/Window.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/Version.h>
#include "InGameConsole.h"
#include "Theme.h"

using namespace OpenRCT2::Ui;

static InGameConsole _inGameConsole;

InGameConsole::InGameConsole()
{
    InteractiveConsole::WriteLine(OPENRCT2_NAME " " OPENRCT2_VERSION);
    InteractiveConsole::WriteLine("Type 'help' for a list of available commands. Type 'hide' to hide the console.");
    InteractiveConsole::WriteLine("");
    WritePrompt();
}

void InGameConsole::WritePrompt()
{
    InteractiveConsole::WriteLine("> ");
}

void InGameConsole::Input(CONSOLE_INPUT input)
{
    switch (input) {
    case CONSOLE_INPUT_LINE_CLEAR:
        ClearInput();
        RefreshCaret();
        break;
    case CONSOLE_INPUT_LINE_EXECUTE:
        if (_consoleCurrentLine[0] != '\0') {
            HistoryAdd(_consoleCurrentLine);

            // Append text we are executing to prompt line
            _consoleLines.back().append(_consoleCurrentLine);

            Execute(_consoleCurrentLine);
            WritePrompt();
            ClearInput();
            RefreshCaret();
        }
        ScrollToEnd();
        break;
    case CONSOLE_INPUT_HISTORY_PREVIOUS:
        if (_consoleHistoryIndex > 0) {
            _consoleHistoryIndex--;
            memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], CONSOLE_INPUT_SIZE);
        }
        _consoleTextInputSession->Size = strlen(_consoleTextInputSession->Buffer);
        _consoleTextInputSession->Length = utf8_length(_consoleTextInputSession->Buffer);
        _consoleTextInputSession->SelectionStart = strlen(_consoleCurrentLine);
        break;
    case CONSOLE_INPUT_HISTORY_NEXT:
        if (_consoleHistoryIndex < _consoleHistoryCount - 1) {
            _consoleHistoryIndex++;
            memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], CONSOLE_INPUT_SIZE);
            _consoleTextInputSession->Size = strlen(_consoleTextInputSession->Buffer);
            _consoleTextInputSession->Length = utf8_length(_consoleTextInputSession->Buffer);
            _consoleTextInputSession->SelectionStart = strlen(_consoleCurrentLine);
        } else {
            _consoleHistoryIndex = _consoleHistoryCount;
            ClearInput();
        }
        break;
    case CONSOLE_INPUT_SCROLL_PREVIOUS:
    {
        int32_t scrollAmt = GetNumVisibleLines() - 1;
        Scroll(scrollAmt);
        break;
    }
    case CONSOLE_INPUT_SCROLL_NEXT:
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
    _consoleCurrentLine[0] = 0;
    if (_isOpen) {
        context_start_text_input(_consoleCurrentLine, sizeof(_consoleCurrentLine));
    }
}

void InGameConsole::HistoryAdd(const utf8 * src)
{
    if (_consoleHistoryCount >= CONSOLE_HISTORY_SIZE) {
        for (int32_t i = 0; i < _consoleHistoryCount - 1; i++)
            memcpy(_consoleHistory[i], _consoleHistory[i + 1], CONSOLE_INPUT_SIZE);
        _consoleHistoryCount--;
    }
    memcpy(_consoleHistory[_consoleHistoryCount++], src, CONSOLE_INPUT_SIZE);
    _consoleHistoryIndex = _consoleHistoryCount;
}

void InGameConsole::ScrollToEnd()
{
    _consoleScrollPos = std::max<int32_t>(0, (int32_t)_consoleLines.size() - GetNumVisibleLines());
}

void InGameConsole::RefreshCaret()
{
    _consoleCaretTicks = 0;
}

void InGameConsole::Scroll(int32_t linesToScroll)
{
    const int32_t maxVisibleLines = GetNumVisibleLines();
    const int32_t numLines = (int32_t)_consoleLines.size();
    if (numLines > maxVisibleLines)
    {
        int32_t maxScrollValue = numLines - maxVisibleLines;
        _consoleScrollPos     = Math::Clamp<int32_t>(0, _consoleScrollPos - linesToScroll, maxScrollValue);
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
    _isOpen = true;
    ScrollToEnd();
    RefreshCaret();
    _consoleTextInputSession = context_start_text_input(_consoleCurrentLine, sizeof(_consoleCurrentLine));
}

void InGameConsole::Close()
{
    _consoleTextInputSession = nullptr;
    _isOpen = false;
    Invalidate();
    context_stop_text_input();
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

void InGameConsole::WriteLine(const std::string &input, uint32_t colourFormat)
{
    // Include text colour format only for special cases
    // The draw function handles the default text colour differently
    utf8 colourCodepoint[4]{};
    if (colourFormat != FORMAT_WINDOW_COLOUR_2)
        utf8_write_codepoint(colourCodepoint, colourFormat);

    std::string line;
    std::size_t splitPos     = 0;
    std::size_t stringOffset = 0;
    while (splitPos != std::string::npos)
    {
        splitPos = input.find('\n', stringOffset);
        line     = input.substr(stringOffset, splitPos - stringOffset);
        _consoleLines.push_back(colourCodepoint + line);
        stringOffset = splitPos + 1;
    }

    if (_consoleLines.size() > CONSOLE_MAX_LINES)
    {
        const std::size_t linesToErase = _consoleLines.size() - CONSOLE_MAX_LINES;
        _consoleLines.erase(_consoleLines.begin(), _consoleLines.begin() + linesToErase);
    }
}

void InGameConsole::Invalidate() const
{
    gfx_set_dirty_blocks(_consoleLeft, _consoleTop, _consoleRight, _consoleBottom);
}

void InGameConsole::Update()
{
    _consoleLeft = 0;
    _consoleTop = 0;
    _consoleRight = context_get_width();
    _consoleBottom = 322;

    if (_isOpen) {
        // When scrolling the map, the console pixels get copied... therefore invalidate the screen
        rct_window *mainWindow = window_get_main();
        if (mainWindow != nullptr) {
            rct_viewport *mainViewport = window_get_viewport(mainWindow);
            if (mainViewport != nullptr) {
                if (_lastMainViewportX != mainViewport->view_x || _lastMainViewportY != mainViewport->view_y) {
                    _lastMainViewportX = mainViewport->view_x;
                    _lastMainViewportY = mainViewport->view_y;

                    gfx_invalidate_screen();
                }
            }
        }

        // Remove unwanted characters in console input
        utf8_remove_format_codes(_consoleCurrentLine, false);
    }

    // Flash the caret
    _consoleCaretTicks = (_consoleCaretTicks + 1) % 30;
}

void InGameConsole::Draw(rct_drawpixelinfo * dpi) const
{
    if (!_isOpen)
        return;

    // Set font
    gCurrentFontSpriteBase           = (gConfigInterface.console_small_font ? FONT_SPRITE_BASE_SMALL : FONT_SPRITE_BASE_MEDIUM);
    gCurrentFontFlags                = 0;
    uint8_t        textColour          = NOT_TRANSLUCENT(theme_get_colour(WC_CONSOLE, 1));
    const int32_t lineHeight          = font_get_line_height(gCurrentFontSpriteBase);
    const int32_t maxLines            = GetNumVisibleLines();

    // This is something of a hack to ensure the text is actually black
    // as opposed to a desaturated grey
    std::string colourFormatStr;
    if (textColour == COLOUR_BLACK)
    {
        utf8 extraTextFormatCode[4]{};
        utf8_write_codepoint(extraTextFormatCode, FORMAT_BLACK);
        colourFormatStr = extraTextFormatCode;
    }

    // TTF looks far better without the outlines
    if (!LocalisationService_UseTrueTypeFont())
    {
        textColour |= COLOUR_FLAG_OUTLINE;
    }

    Invalidate();

    // Give console area a translucent effect.
    gfx_filter_rect(dpi, _consoleLeft, _consoleTop, _consoleRight, _consoleBottom, PALETTE_51);

    // Make input area more opaque.
    gfx_filter_rect(dpi, _consoleLeft, _consoleBottom - lineHeight - 10, _consoleRight, _consoleBottom - 1, PALETTE_51);

    // Paint background colour.
    uint8_t backgroundColour = theme_get_colour(WC_CONSOLE, 0);
    gfx_fill_rect_inset(dpi, _consoleLeft, _consoleTop, _consoleRight, _consoleBottom, backgroundColour, INSET_RECT_FLAG_FILL_NONE);
    gfx_fill_rect_inset(dpi, _consoleLeft + 1, _consoleTop + 1, _consoleRight - 1, _consoleBottom - 1, backgroundColour, INSET_RECT_FLAG_BORDER_INSET);

    std::string lineBuffer;
    int32_t      x = _consoleLeft + CONSOLE_EDGE_PADDING;
    int32_t      y = _consoleTop + CONSOLE_EDGE_PADDING;

    // Draw text inside console
    for (std::size_t i = 0; i < _consoleLines.size() && i < (size_t)maxLines; i++) {
        const size_t index = i + _consoleScrollPos;
        lineBuffer         = colourFormatStr + _consoleLines[index];
        gfx_draw_string(dpi, lineBuffer.c_str(), textColour, x, y);
        y += lineHeight;
    }

    y = _consoleBottom - lineHeight - CONSOLE_EDGE_PADDING - 1;

    // Draw current line
    lineBuffer = colourFormatStr + _consoleCurrentLine;
    gfx_draw_string(dpi, lineBuffer.c_str(), TEXT_COLOUR_255, x, y);

    // Draw caret
    if (_consoleCaretTicks < CONSOLE_CARET_FLASH_THRESHOLD) {
        int32_t caretX = x + gfx_get_string_width(_consoleCurrentLine);
        int32_t caretY = y + lineHeight;

        uint8_t caretColour = ColourMapA[BASE_COLOUR(textColour)].lightest;
        gfx_fill_rect(dpi, caretX, caretY, caretX + CONSOLE_CARET_WIDTH, caretY, caretColour);
    }

    // What about border colours?
    uint8_t borderColour1 = ColourMapA[BASE_COLOUR(backgroundColour)].light;
    uint8_t borderColour2 = ColourMapA[BASE_COLOUR(backgroundColour)].mid_dark;

    // Input area top border
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - lineHeight - 11, _consoleRight, _consoleBottom - lineHeight - 11, borderColour1);
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - lineHeight - 10, _consoleRight, _consoleBottom - lineHeight - 10, borderColour2);

    // Input area bottom border
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 1, _consoleRight, _consoleBottom - 1, borderColour1);
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 0, _consoleRight, _consoleBottom - 0, borderColour2);
}

// Calculates the amount of visible lines, based on the console size, excluding the input line.
int32_t InGameConsole::GetNumVisibleLines() const
{
    const int32_t lineHeight     = font_get_line_height(gCurrentFontSpriteBase);
    const int32_t consoleHeight  = _consoleBottom - _consoleTop;
    const int32_t drawableHeight = consoleHeight - 2 * lineHeight - 4; // input line, separator - padding
    return drawableHeight / lineHeight;
}
