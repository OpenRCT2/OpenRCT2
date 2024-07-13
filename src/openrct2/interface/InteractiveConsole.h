/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/FormatCodes.h"

#include <cstdint>
#include <string>

struct DrawPixelInfo;
struct TextInputSession;

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

class InteractiveConsole
{
public:
    virtual ~InteractiveConsole()
    {
    }

    void Execute(const std::string& s);
    void WriteLine(const std::string& s);
    void WriteLineError(const std::string& s);
    void WriteLineWarning(const std::string& s);
    void WriteFormatLine(const char* format, ...);

    virtual void Clear() = 0;
    virtual void Close() = 0;
    virtual void Hide() = 0;
    virtual void WriteLine(const std::string& s, FormatToken colourFormat) = 0;
};
