/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

// Ignore isatty warning on WIN32
#ifndef _CRT_NONSTDC_NO_WARNINGS
#    define _CRT_NONSTDC_NO_WARNINGS
#endif

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../platform/Platform.h"
#include "../scripting/ScriptEngine.h"
#include "InteractiveConsole.h"

#include <linenoise.hpp>
namespace OpenRCT2
{

    void StdInOutConsole::Start()
    {
        // Only start if stdin/stdout is a TTY
        if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
        {
            return;
        }

        std::thread replThread([this]() -> void {
            linenoise::SetMultiLine(true);
            linenoise::SetHistoryMaxLen(32);

            std::string prompt = "\033[32mopenrct2 $\x1b[0m ";
            bool lastPromptQuit = false;
            while (true)
            {
                std::string line;
                std::string left = prompt;
                _isPromptShowing = true;
                auto quit = linenoise::Readline(left.c_str(), line);
                _isPromptShowing = false;
                if (quit)
                {
                    if (lastPromptQuit)
                    {
                        OpenRCT2Finish();
                        break;
                    }

                    lastPromptQuit = true;
                    std::puts("(To exit, press ^C again)");
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

    std::future<void> StdInOutConsole::Eval(const std::string& s)
    {
#ifdef ENABLE_SCRIPTING
        auto& scriptEngine = GetContext()->GetScriptEngine();
        return scriptEngine.Eval(s);
#else
        // Push on-demand evaluations onto a queue so that it can be processed deterministically
        // on the main thead at the right time.
        std::promise<void> barrier;
        auto future = barrier.get_future();
        _evalQueue.emplace(std::move(barrier), s);
        return future;
#endif
    }

    void StdInOutConsole::ProcessEvalQueue()
    {
#ifndef ENABLE_SCRIPTING
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
#endif
    }

    void StdInOutConsole::Clear()
    {
        linenoise::linenoiseClearScreen();
    }

    void StdInOutConsole::Close()
    {
        OpenRCT2Finish();
    }

    void StdInOutConsole::WriteLine(const std::string& s, FormatToken colourFormat)
    {
        std::string formatBegin;
        switch (colourFormat)
        {
            case FormatToken::ColourRed:
                formatBegin = "\033[31m";
                break;
            case FormatToken::ColourYellow:
                formatBegin = "\033[33m";
                break;
            default:
                break;
        }

        if (!Platform::IsColourTerminalSupported())
        {
            std::printf("%s\n", s.c_str());
            std::fflush(stdout);
        }
        else
        {
            if (_isPromptShowing)
            {
                auto* mainString = s.c_str();

                // If string contains \n, we need to replace with \r\n
                std::string newString;
                if (s.find('\n') != std::string::npos)
                {
                    for (auto ch : s)
                    {
                        if (ch == '\n')
                            newString += "\r\n";
                        else
                            newString += ch;
                    }
                    mainString = newString.c_str();
                }

                std::printf("\r%s%s\x1b[0m\x1b[0K\r\n", formatBegin.c_str(), mainString);
                std::fflush(stdout);
                linenoise::linenoiseEditRefreshLine();
            }
            else
            {
                std::printf("%s%s\x1b[0m\n", formatBegin.c_str(), s.c_str());
                std::fflush(stdout);
            }
        }
    }
} // namespace OpenRCT2
