/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

// NOLINTBEGIN
#if defined(_MSC_VER)
    #include <intrin.h>
    #define debug_break() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
    #define debug_break() __builtin_trap()
#else
    #include <signal.h>
    #define debug_break() raise(SIGTRAP)
#endif
// NOLINTEND

#include "../Version.h"
#include "Console.hpp"
#include "Diagnostics.hpp"
#include "Guard.hpp"
#include "String.hpp"
#include "StringBuilder.h"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <sstream>

namespace OpenRCT2::Guard
{
    static constexpr const utf8* kAssertionMessage = "An assertion failed, please report this to the OpenRCT2 developers.";

    // The default behaviour when an assertion is raised.
    static AssertBehaviour _assertBehaviour =
#ifdef _WIN32
        AssertBehaviour::messageBox
#else
        AssertBehaviour::cAssert
#endif
        ;

    static std::optional<std::string> _lastAssertMessage = std::nullopt;

#ifdef _WIN32
    [[nodiscard]] static std::wstring CreateDialogAssertMessage(std::string_view);
    static void ForceCrash();
#endif

    AssertBehaviour GetAssertBehaviour()
    {
        return _assertBehaviour;
    }

    void SetAssertBehaviour(AssertBehaviour behaviour)
    {
        _assertBehaviour = behaviour;
    }

    void Assert(bool expression, const std::source_location& location)
    {
        if (expression)
            return;

        std::stringstream messageStream;
        messageStream << "Assertion failed in " << location.file_name() << ":" << location.line();

        std::string message = messageStream.str();

        Assert(expression, message.c_str());
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

        Console::Error::WriteLine(kAssertionMessage);
        Console::Error::WriteLine("Version: %s", gVersionInfoFull);

        // This is never freed, but acceptable considering we are about to crash out
        std::string formattedMessage;
        if (message != nullptr)
        {
            formattedMessage = String::formatVA(message, args);
            Console::Error::WriteLine(formattedMessage.c_str());
            _lastAssertMessage = std::make_optional(formattedMessage);
        }

#ifdef DEBUG
        Debug::Break();
#endif

        switch (_assertBehaviour)
        {
            case AssertBehaviour::abort:
                abort();
            default:
            case AssertBehaviour::cAssert:
                assert(false);
                break;
#ifdef _WIN32
            case AssertBehaviour::messageBox:
            {
                // Show message box if we are not building for testing
                auto buffer = CreateDialogAssertMessage(formattedMessage);
                int32_t result = MessageBoxW(
                    nullptr, buffer.c_str(), L"" OPENRCT2_NAME, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION);
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
    [[nodiscard]] static std::wstring CreateDialogAssertMessage(std::string_view formattedMessage)
    {
        StringBuilder sb;
        sb.Append(kAssertionMessage);
        sb.Append("\n\n");
        sb.Append("Version: ");
        sb.Append(gVersionInfoFull);
        if (!formattedMessage.empty())
        {
            sb.Append("\n");
            sb.Append(formattedMessage);
        }
        return String::toWideChar({ sb.GetBuffer(), sb.GetLength() });
    }

    static void ForceCrash()
    {
        debug_break();
    }
#endif
} // namespace OpenRCT2::Guard
