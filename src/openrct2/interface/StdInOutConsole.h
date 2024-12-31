/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "InteractiveConsole.h"

#include <atomic>
#include <future>
#include <queue>

class StdInOutConsole final : public InteractiveConsole
{
private:
    std::queue<std::tuple<std::promise<void>, std::string>> _evalQueue;
    std::atomic<bool> _isPromptShowing{};

public:
    void Start();
    std::future<void> Eval(const std::string& s);
    void ProcessEvalQueue();

    void Clear() override;
    void Close() override;
    void Hide() override
    {
    }
    void WriteLine(const std::string& s)
    {
        InteractiveConsole::WriteLine(s);
    }
    void WriteLine(const std::string& s, FormatToken colourFormat) override;
};
