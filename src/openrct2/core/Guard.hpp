/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
