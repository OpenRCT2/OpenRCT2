#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include <cassert>
#include <stdarg.h>
#include <stdio.h>

#include "../common.h"

#ifdef __WINDOWS__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetMessage
#endif

#include "../Version.h"
#include "Console.hpp"
#include "Diagnostics.hpp"
#include "Guard.hpp"
#include "String.hpp"

extern "C"
{
    void openrct2_assert(bool expression, const char * message, ...)
    {
        va_list args;
        va_start(args, message);
        Guard::Assert_VA(expression, message, args);
        va_end(args);
    }

}

namespace Guard
{
    constexpr const utf8 * ASSERTION_MESSAGE = "An assertion failed, please report this to the OpenRCT2 developers.";

    // The default behaviour when an assertion is raised.
    static ASSERT_BEHAVIOUR _assertBehaviour =
#ifdef __WINDOWS__
        ASSERT_BEHAVIOUR::MESSAGE_BOX
#else
        ASSERT_BEHAVIOUR::CASSERT
#endif
        ;

#ifdef __WINDOWS__
    static void GetAssertMessage(char * buffer, size_t bufferSize, const char * formattedMessage);
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

    void Assert(bool expression, const char * message, ...)
    {
        va_list args;
        va_start(args, message);
        Assert_VA(expression, message, args);
        va_end(args);
    }

    void Assert_VA(bool expression, const char * message, va_list args)
    {
        if (expression) return;

        Console::Error::WriteLine(ASSERTION_MESSAGE);
        Console::Error::WriteLine("Version: %s", gVersionInfoFull);

        // This is never freed, but acceptable considering we are about to crash out
        utf8 * formattedMessage = nullptr;
        if (message != nullptr)
        {
            formattedMessage = String::Format_VA(message, args);
            Console::Error::WriteLine(formattedMessage);
        }

#ifdef DEBUG
        Debug::Break();
#endif

        switch (_assertBehaviour) {
        case ASSERT_BEHAVIOUR::ABORT:
            abort();
            break;
        default:
        case ASSERT_BEHAVIOUR::CASSERT:
            assert(false);
            break;
#ifdef __WINDOWS__
        case ASSERT_BEHAVIOUR::MESSAGE_BOX:
        {
            // Show message box if we are not building for testing
            char buffer[512];
            GetAssertMessage(buffer, sizeof(buffer), formattedMessage);
            sint32 result = MessageBoxA(nullptr, buffer, OPENRCT2_NAME, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION);
            if (result == IDABORT)
            {
                ForceCrash();
            }
            break;
        }
#endif
        }
    }

    void Fail(const char * message, ...)
    {
        va_list args;
        va_start(args, message);
        Assert_VA(false, message, args);
        va_end(args);
    }

    void Fail_VA(const char * message, va_list args)
    {
        Assert_VA(false, message, args);
    }

#ifdef __WINDOWS__
    static void GetAssertMessage(char * buffer, size_t bufferSize, const char * formattedMessage)
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
#ifdef USE_BREAKPAD
        // Force a crash that breakpad will handle allowing us to get a dump
        *((void**)0) = 0;
#else
        assert(false);
#endif
    }
#endif
}
