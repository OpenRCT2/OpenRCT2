/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef _WIN32
#    include <windows.h>
#endif

#include "../Version.h"
#include "../common.h"
#include "Console.hpp"
#include "Diagnostics.hpp"
#include "Guard.hpp"
#include "String.hpp"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

void openrct2_assert_fwd(bool expression, const char* message, ...)
{
    va_list va;
    va_start(va, message);
    Guard::Assert_VA(expression, message, va);
    va_end(va);
}

namespace Guard
{
    constexpr const utf8* ASSERTION_MESSAGE = "An assertion failed, please report this to the OpenRCT2 developers.";

    // The default behaviour when an assertion is raised.
    static ASSERT_BEHAVIOUR _assertBehaviour =
#ifdef _WIN32
        ASSERT_BEHAVIOUR::MESSAGE_BOX
#else
        ASSERT_BEHAVIOUR::CASSERT
#endif
        ;

    static std::optional<std::string> _lastAssertMessage = std::nullopt;

#ifdef _WIN32
    static void GetAssertMessage(char* buffer, size_t bufferSize, const char* formattedMessage);
    static void ForceCrash();
#endif

    ASSERT_BEHAVIOUR GetAssertBehaviour()
    {
        return _assertBehaviour;
    }

    void SetAssertBehaviour(ASSERT_BEHAVIOUR behaviour)
    {
        _assertBehaviour = behaviour;
    }

    void Assert(bool expression, const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        Assert_VA(expression, message, args);
        va_end(args);
    }

    void Assert_VA(bool expression, const char* message, va_list args)
    {
        if (expression)
            return;

        Console::Error::WriteLine(ASSERTION_MESSAGE);
        Console::Error::WriteLine("Version: %s", gVersionInfoFull);

        // This is never freed, but acceptable considering we are about to crash out
        utf8* formattedMessage = nullptr;
        if (message != nullptr)
        {
            formattedMessage = String::Format_VA(message, args);
            Console::Error::WriteLine(formattedMessage);
        }

        if (formattedMessage != nullptr)
        {
            _lastAssertMessage = std::make_optional(formattedMessage);
        }

#ifdef DEBUG
        Debug::Break();
#endif

        switch (_assertBehaviour)
        {
            case ASSERT_BEHAVIOUR::ABORT:
                abort();
            default:
            case ASSERT_BEHAVIOUR::CASSERT:
                assert(false);
                break;
#ifdef _WIN32
            case ASSERT_BEHAVIOUR::MESSAGE_BOX:
            {
                // Show message box if we are not building for testing
                char buffer[512];
                GetAssertMessage(buffer, sizeof(buffer), formattedMessage);
                int32_t result = MessageBoxA(nullptr, buffer, OPENRCT2_NAME, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION);
                if (result == IDABORT)
                {
                    ForceCrash();
                }
                break;
            }
#endif
        }
    }

    void Fail(const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        Assert_VA(false, message, args);
        va_end(args);
    }

    void Fail_VA(const char* message, va_list args)
    {
        Assert_VA(false, message, args);
    }

    std::optional<std::string> GetLastAssertMessage()
    {
        return _lastAssertMessage;
    }

#ifdef _WIN32
    static void GetAssertMessage(char* buffer, size_t bufferSize, const char* formattedMessage)
    {
        String::Set(buffer, bufferSize, ASSERTION_MESSAGE);
        String::Append(buffer, bufferSize, "\r\n\r\n");
        String::Append(buffer, bufferSize, "Version: ");
        String::Append(buffer, bufferSize, gVersionInfoFull);
        if (formattedMessage != nullptr)
        {
            String::Append(buffer, bufferSize, "\r\n");
            String::Append(buffer, bufferSize, formattedMessage);
        }
    }

    static void ForceCrash()
    {
#    ifdef USE_BREAKPAD
        // Force a crash that breakpad will handle allowing us to get a dump
        *((void**)0) = 0;
#    else
        assert(false);
#    endif
    }
#endif
} // namespace Guard
