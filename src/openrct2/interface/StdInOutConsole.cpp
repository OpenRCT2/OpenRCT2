/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../thirdparty/linenoise.hpp"
#include "../OpenRCT2.h"
#include "../platform/Platform2.h"
#include "InteractiveConsole.h"

void StdInOutConsole::Start()
{
    std::thread replThread ([this]() -> void
    {
        linenoise::SetMultiLine(true);
        linenoise::SetHistoryMaxLen(32);

        std::string prompt = "\033[32mopenrct2 $\x1b[0m ";
        bool lastPromptQuit = false;
        while (true)
        {
            std::string line;
            std::string left = prompt;
            auto quit = linenoise::Readline(left.c_str(), line);
            if (quit)
            {
                if (lastPromptQuit)
                {
                    openrct2_finish();
                    break;
                }
                else
                {
                    lastPromptQuit = true;
                    std::puts("(To exit, press ^C again or type exit)");
                }
            }
            else
            {
                lastPromptQuit = false;
                linenoise::AddHistory(line.c_str());
                Eval(line).wait();
            }
        }
    });
    replThread.detach();
}

std::future<void> StdInOutConsole::Eval(const std::string &s)
{
    // Push on-demand evaluations onto a queue so that it can be processed deterministically
    // on the main thead at the right time.
    std::promise<void> barrier;
    auto future = barrier.get_future();
    _evalQueue.emplace(std::move(barrier), s);
    return future;
}

void StdInOutConsole::ProcessEvalQueue()
{
    while (_evalQueue.size() > 0)
    {
        auto item = std::move(_evalQueue.front());
        _evalQueue.pop();
        auto promise = std::move(std::get<0>(item));
        auto command = std::move(std::get<1>(item));

        Execute(command);

        // Signal the promise so caller can continue
        promise.set_value();
    }
}

void StdInOutConsole::Clear()
{
    linenoise::linenoiseClearScreen();
}

void StdInOutConsole::Close()
{
    openrct2_finish();
}

void StdInOutConsole::WriteLine(const std::string &s, uint32_t colourFormat)
{
    std::string formatBegin;
    if (colourFormat != FORMAT_WINDOW_COLOUR_2)
    {
        switch (colourFormat)
        {
            case FORMAT_RED:
                formatBegin = "\033[31m";
                break;
            case FORMAT_YELLOW:
                formatBegin = "\033[33m";
                break;
        }
    }

    if (formatBegin.empty() || !Platform::IsColourTerminalSupported())
    {
        std::printf("%s\n", s.c_str());
    }
    else
    {
        std::printf("%s%s%s\n", formatBegin.c_str(), s.c_str(), "\x1b[0m");
    }
}
