#include <algorithm>
#include "Colour.h"
#include "Console.h"
#include "themes.h"
#include "Window.h"
#include "../config/Config.h"
#include "../Context.h"
#include "../core/Math.hpp"
#include "../localisation/Language.h"
#include "../Version.h"

#define CONSOLE_MAX_LINES 300
#define CONSOLE_HISTORY_SIZE 64
#define CONSOLE_INPUT_SIZE 256
#define CONSOLE_CARET_FLASH_THRESHOLD 15
#define CONSOLE_EDGE_PADDING 4
#define CONSOLE_CARET_WIDTH 6

bool gConsoleOpen = false;

static sint32                   _consoleLeft, _consoleTop, _consoleRight, _consoleBottom;
static sint32                   _lastMainViewportX, _lastMainViewportY;
static std::deque<std::string>  _consoleLines;
static utf8                     _consoleCurrentLine[CONSOLE_INPUT_SIZE];
static sint32                   _consoleCaretTicks;
static sint32                   _consoleScrollPos = 0;
static TextInputSession *       _consoleTextInputSession;

static utf8   _consoleHistory[CONSOLE_HISTORY_SIZE][CONSOLE_INPUT_SIZE];
static sint32 _consoleHistoryIndex = 0;
static sint32 _consoleHistoryCount = 0;

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
            memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], 256);
        }
        _consoleTextInputSession->Size = strlen(_consoleTextInputSession->Buffer);
        _consoleTextInputSession->Length = utf8_length(_consoleTextInputSession->Buffer);
        _consoleTextInputSession->SelectionStart = strlen(_consoleCurrentLine);
        break;
    case CONSOLE_INPUT_HISTORY_NEXT:
        if (_consoleHistoryIndex < _consoleHistoryCount - 1) {
            _consoleHistoryIndex++;
            memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], 256);
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
        sint32 scrollAmt = GetNumVisibleLines() - 1;
        Scroll(scrollAmt);
        break;
    }
    case CONSOLE_INPUT_SCROLL_NEXT:
    {
        sint32 scrollAmt = GetNumVisibleLines() - 1;
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
    if (gConsoleOpen) {
        context_start_text_input(_consoleCurrentLine, sizeof(_consoleCurrentLine));
    }
}

void InGameConsole::HistoryAdd(const utf8 * src)
{
    if (_consoleHistoryCount >= CONSOLE_HISTORY_SIZE) {
        for (sint32 i = 0; i < _consoleHistoryCount - 1; i++)
            memcpy(_consoleHistory[i], _consoleHistory[i + 1], CONSOLE_INPUT_SIZE);
        _consoleHistoryCount--;
    }
    memcpy(_consoleHistory[_consoleHistoryCount++], src, CONSOLE_INPUT_SIZE);
    _consoleHistoryIndex = _consoleHistoryCount;
}

void InGameConsole::ScrollToEnd()
{
    _consoleScrollPos = std::max<sint32>(0, (sint32)_consoleLines.size() - GetNumVisibleLines());
}

void InGameConsole::RefreshCaret()
{
    _consoleCaretTicks = 0;
}

void InGameConsole::Scroll(sint32 linesToScroll)
{
    const sint32 maxVisibleLines = GetNumVisibleLines();
    const sint32 numLines = (sint32)_consoleLines.size();
    if (numLines > maxVisibleLines)
    {
        sint32 maxScrollValue = numLines - maxVisibleLines;
        _consoleScrollPos     = Math::Clamp<sint32>(0, _consoleScrollPos - linesToScroll, maxScrollValue);
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
    gConsoleOpen = true;
    ScrollToEnd();
    RefreshCaret();
    _consoleTextInputSession = context_start_text_input(_consoleCurrentLine, sizeof(_consoleCurrentLine));
}

void InGameConsole::Close()
{
    gConsoleOpen = false;
    Invalidate();
    context_stop_text_input();
}

void InGameConsole::Toggle()
{
    if (gConsoleOpen)
        console_close();
    else
        console_open();
}

void InGameConsole::WriteLine(const std::string &input, uint32 colourFormat)
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

void InGameConsole::Invalidate()
{
    gfx_set_dirty_blocks(_consoleLeft, _consoleTop, _consoleRight, _consoleBottom);
}

void InGameConsole::Update()
{
    _consoleLeft = 0;
    _consoleTop = 0;
    _consoleRight = context_get_width();
    _consoleBottom = 322;

    if (gConsoleOpen) {
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

void InGameConsole::Draw(rct_drawpixelinfo * dpi)
{
    if (!gConsoleOpen)
        return;

    // Set font
    gCurrentFontSpriteBase           = (gConfigInterface.console_small_font ? FONT_SPRITE_BASE_SMALL : FONT_SPRITE_BASE_MEDIUM);
    gCurrentFontFlags                = 0;
    uint8        textColour          = NOT_TRANSLUCENT(theme_get_colour(WC_CONSOLE, 1));
    const sint32 lineHeight          = font_get_line_height(gCurrentFontSpriteBase);
    const sint32 maxLines            = GetNumVisibleLines();

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
    if (!gUseTrueTypeFont)
    {
        textColour |= COLOUR_FLAG_OUTLINE;
    }

    Invalidate();

    // Give console area a translucent effect.
    gfx_filter_rect(dpi, _consoleLeft, _consoleTop, _consoleRight, _consoleBottom, PALETTE_51);

    // Make input area more opaque.
    gfx_filter_rect(dpi, _consoleLeft, _consoleBottom - lineHeight - 10, _consoleRight, _consoleBottom - 1, PALETTE_51);

    // Paint background colour.
    uint8 backgroundColour = theme_get_colour(WC_CONSOLE, 0);
    gfx_fill_rect_inset(dpi, _consoleLeft, _consoleTop, _consoleRight, _consoleBottom, backgroundColour, INSET_RECT_FLAG_FILL_NONE);
    gfx_fill_rect_inset(dpi, _consoleLeft + 1, _consoleTop + 1, _consoleRight - 1, _consoleBottom - 1, backgroundColour, INSET_RECT_FLAG_BORDER_INSET);

    std::string lineBuffer;
    sint32      x = _consoleLeft + CONSOLE_EDGE_PADDING;
    sint32      y = _consoleTop + CONSOLE_EDGE_PADDING;

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
        sint32 caretX = x + gfx_get_string_width(_consoleCurrentLine);
        sint32 caretY = y + lineHeight;

        uint8 caretColour = ColourMapA[BASE_COLOUR(textColour)].lightest;
        gfx_fill_rect(dpi, caretX, caretY, caretX + CONSOLE_CARET_WIDTH, caretY, caretColour);
    }

    // What about border colours?
    uint8 borderColour1 = ColourMapA[BASE_COLOUR(backgroundColour)].light;
    uint8 borderColour2 = ColourMapA[BASE_COLOUR(backgroundColour)].mid_dark;

    // Input area top border
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - lineHeight - 11, _consoleRight, _consoleBottom - lineHeight - 11, borderColour1);
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - lineHeight - 10, _consoleRight, _consoleBottom - lineHeight - 10, borderColour2);

    // Input area bottom border
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 1, _consoleRight, _consoleBottom - 1, borderColour1);
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 0, _consoleRight, _consoleBottom - 0, borderColour2);
}

// Calculates the amount of visible lines, based on the console size, excluding the input line.
sint32 InGameConsole::GetNumVisibleLines()
{
    const sint32 lineHeight     = font_get_line_height(gCurrentFontSpriteBase);
    const sint32 consoleHeight  = _consoleBottom - _consoleTop;
    const sint32 drawableHeight = consoleHeight - 2 * lineHeight - 4; // input line, separator - padding
    return drawableHeight / lineHeight;
}

// Old pass-through functions

void console_open()
{
    _inGameConsole.Open();
}

void console_close()
{
    _inGameConsole.Close();
}

void console_toggle()
{
    _inGameConsole.Toggle();
}

void console_update()
{
    _inGameConsole.Update();
}

void console_draw(rct_drawpixelinfo *dpi)
{
    _inGameConsole.Draw(dpi);
}

void console_input(CONSOLE_INPUT input)
{
    _inGameConsole.Input(input);
}

void console_writeline(const utf8 * src, uint32 colourFormat)
{
    _inGameConsole.WriteLine(src, colourFormat);
}

void console_scroll(sint32 linesToScroll)
{
    _inGameConsole.Scroll(linesToScroll);
}

void console_refresh_caret()
{
    _inGameConsole.RefreshCaret();
}
