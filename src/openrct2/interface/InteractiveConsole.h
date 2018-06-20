/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <deque>
#include <future>
#include <queue>
#include <string>
#include <tuple>
#include "../common.h"
#include "../localisation/FormatCodes.h"

struct rct_drawpixelinfo;
struct TextInputSession;

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

    void Execute(const std::string &s);
    void WriteLine(const std::string &s);
    void WriteLineError(const std::string &s);
    void WriteLineWarning(const std::string &s);
    void WriteFormatLine(const char * format, ...);

    virtual void Clear() abstract;
    virtual void Close() abstract;
    virtual void Hide() abstract;
    virtual void WriteLine(const std::string &s, uint32_t colourFormat) abstract;
};

class StdInOutConsole final : public InteractiveConsole
{
private:
    std::queue<std::tuple<std::promise<void>, std::string>> _evalQueue;
public:
    void Start();
    std::future<void> Eval(const std::string &s);
    void ProcessEvalQueue();

    void Clear() override;
    void Close() override;
    void Hide() override { }
    void WriteLine(const std::string &s) { InteractiveConsole::WriteLine(s); }
    void WriteLine(const std::string &s, uint32_t colourFormat) override;
};
