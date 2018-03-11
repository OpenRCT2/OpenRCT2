#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#pragma once

#include <future>
#include <queue>
#include <string>
#include <tuple>
#include "../common.h"
#include "../localisation/FormatCodes.h"

struct rct_drawpixelinfo;

enum CONSOLE_INPUT
{
    CONSOLE_INPUT_NONE,
    CONSOLE_INPUT_LINE_CLEAR,
    CONSOLE_INPUT_LINE_EXECUTE,
    CONSOLE_INPUT_HISTORY_PREVIOUS,
    CONSOLE_INPUT_HISTORY_NEXT,
    CONSOLE_INPUT_SCROLL_PREVIOUS,
    CONSOLE_INPUT_SCROLL_NEXT,
};

class InteractiveConsole
{
public:
    virtual ~InteractiveConsole() { }

    sint32 cc_clear(const utf8 **argv, sint32 argc);

    void Execute(const std::string &s);
    void WriteLine(const std::string &s);
    void WriteLineError(const std::string &s);
    void WriteLineWarning(const std::string &s);
    void WriteFormatLine(const std::string &format, ...);

    virtual void Clear() abstract;
    virtual void Close() abstract;
    virtual void WriteLine(const std::string &s, uint32 colourFormat) abstract;
};

class InGameConsole : public InteractiveConsole
{
public:
    InGameConsole();

    void Clear() override;
    void Open();
    void Close() override;
    void Toggle();
    void WriteLine(const std::string &s, uint32 colourFormat) override;

    void Input(CONSOLE_INPUT input);
    void RefreshCaret();
    void Scroll(sint32 linesToScroll);

    void Update();
    void Draw(rct_drawpixelinfo * dpi);

private:
    void ClearInput();
    void ClearLine();
    void HistoryAdd(const utf8 * src);
    void WritePrompt();
    void ScrollToEnd();
    void Invalidate();
    sint32 GetNumVisibleLines();
};

class StdInOutConsole : public InteractiveConsole
{
private:
    std::queue<std::tuple<std::promise<void>, std::string>> _evalQueue;
public:
    void Start();
    std::future<void> Eval(const std::string &s);
    void ProcessEvalQueue();

    void Clear() override;
    void Close() override;
    void WriteLine(const std::string &s, uint32 colourFormat) override;
};

// Old pass-through functions for in-game console
extern bool gConsoleOpen;

void console_open();
void console_close();
void console_toggle();
void console_update();
void console_draw(rct_drawpixelinfo *dpi);
void console_input(CONSOLE_INPUT input);
void console_writeline(const utf8 * src, uint32 colourFormat = FORMAT_WINDOW_COLOUR_2);
void console_refresh_caret();
void console_scroll(sint32 delta);
