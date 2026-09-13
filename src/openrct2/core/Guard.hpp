/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <optional>
#include <source_location>
#include <stdarg.h>
#include <stdbool.h>
#include <string>

enum class AssertBehaviour
{
    abort,
    cAssert,
    messageBox,
};

/**
 * Utility methods for asserting function parameters.
 */
namespace OpenRCT2::Guard
{
    AssertBehaviour GetAssertBehaviour();
    void SetAssertBehaviour(AssertBehaviour behaviour);

    void Assert(bool expression, const std::source_location& location = std::source_location::current());
    void Assert(bool expression, const char* message, ...);
    void Assert_VA(bool expression, const char* message, va_list args);
    void Fail(const char* message = nullptr, ...);
    void Fail_VA(const char* message, va_list args);

    std::optional<std::string> GetLastAssertMessage();

    template<typename T>
    static void ArgumentNotNull(T* argument, const char* message = nullptr, ...)
    {
        va_list args;
        va_start(args, message);
        Assert_VA(argument != nullptr, message, args);
        va_end(args);
    }

    template<typename T>
    static void ArgumentNotNull(const std::shared_ptr<T>& argument, const char* message = nullptr, ...)
    {
        va_list args;
        va_start(args, message);
        Assert_VA(argument != nullptr, message, args);
        va_end(args);
    }

    template<typename T>
    static void ArgumentInRange(T argument, T min, T max, const char* message = nullptr, ...)
    {
        va_list args;
        va_start(args, message);
        Assert(argument >= min && argument <= max, message, args);
        va_end(args);
    }

    template<typename T>
    static void IndexInRange(size_t index, const T& container)
    {
        Guard::Assert(index < container.size(), "Index %zu out of bounds (%zu)", index, container.size());
    }

} // namespace OpenRCT2::Guard

#define GUARD_LINE "Location: %s:%d", __func__, __LINE__
