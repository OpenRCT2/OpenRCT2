/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <atomic>
#include <cstdint>
#include <string>

enum class ConsoleInput : uint8_t
{
    none,
    lineClear,
    lineExecute,
    historyPrevious,
    historyNext,
    scrollPrevious,
    scrollNext,
};

namespace OpenRCT2
{
    enum class FormatToken : uint8_t;
} // namespace OpenRCT2

class InteractiveConsole
{
private:
    std::atomic_flag _commandExecuting;

public:
    virtual ~InteractiveConsole()
    {
    }

    void Execute(const std::string& s);
    void WriteLine(std::string_view s);
    void WriteLineError(std::string_view s);
    void WriteLineWarning(std::string_view s);
    void WriteFormatLine(const char* format, ...);

    void BeginAsyncExecution();
    void EndAsyncExecution();

    bool IsExecuting();

    virtual void Clear() = 0;
    virtual void Close() = 0;
    virtual void Hide() = 0;
    virtual void WriteLine(std::string_view s, OpenRCT2::FormatToken colourFormat) = 0;
};
