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

#pragma once

#include <memory>
#include <stdarg.h>
#include <stdbool.h>

void openrct2_assert_fwd(bool expression, const char * message, ...);

#define openrct2_assert(expr, msg, ...) if(!(expr)) { openrct2_assert_fwd((expr), msg, ##__VA_ARGS__); }

enum class ASSERT_BEHAVIOUR
{
    ABORT,
    CASSERT,
    MESSAGE_BOX,
};

/**
 * Utility methods for asserting function parameters.
 */
namespace Guard
{
    ASSERT_BEHAVIOUR GetAssertBehaviour();
    void SetAssertBehaviour(ASSERT_BEHAVIOUR behaviour);

    void Assert(bool expression, const char * message = nullptr, ...);
    void Assert_VA(bool expression, const char * message, va_list args);
    void Fail(const char * message = nullptr, ...);
    void Fail_VA(const char * message, va_list args);

    template<typename T>
    static void ArgumentNotNull(T * argument, const char * message = nullptr, ...)
    {
        va_list args;
        va_start(args, message);
        Assert_VA(argument != nullptr, message, args);
        va_end(args);
    }

    template<typename T>
    static void ArgumentNotNull(const std::shared_ptr<T>& argument, const char * message = nullptr, ...)
    {
        va_list args;
        va_start(args, message);
        Assert_VA(argument != nullptr, message, args);
        va_end(args);
    }

    template<typename T>
    static void ArgumentInRange(T argument, T min, T max, const char * message = nullptr, ...)
    {
        va_list args;
        va_start(args, message);
        Assert(argument >= min && argument <= max, message, args);
        va_end(args);
    }
} // namespace Guard

#define GUARD_LINE "Location: %s:%d", __func__, __LINE__
