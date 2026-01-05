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
    None,
    LineClear,
    LineExecute,
    HistoryPrevious,
    HistoryNext,
    ScrollPrevious,
    ScrollNext,
};

namespace OpenRCT2
{
    enum class FormatToken : uint8_t;
    struct TextInputSession;
} // namespace OpenRCT2

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

class InteractiveConsole
{
private:
    std::atomic_flag _commandExecuting;

public:
    virtual ~InteractiveConsole()
    {
    }

    void Execute(const std::string& s);
    void WriteLine(const std::string& s);
    void WriteLineError(const std::string& s);
    void WriteLineWarning(const std::string& s);
    void WriteFormatLine(const char* format, ...);

    void BeginAsyncExecution();
    void EndAsyncExecution();

    bool IsExecuting();

    virtual void Clear() = 0;
    virtual void Close() = 0;
    virtual void Hide() = 0;
    virtual void WriteLine(const std::string& s, OpenRCT2::FormatToken colourFormat) = 0;
};
