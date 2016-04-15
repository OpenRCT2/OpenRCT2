#pragma once

#include <cassert>
#include <stdio.h>

#include "Console.hpp"
#include "Diagnostics.hpp"

/**
 * Utility methods for asserting function parameters.
 */
namespace Guard
{
    void Assert(bool expression, const char * message = nullptr)
    {
        if (expression) return;

        if (message != nullptr)
        {
            Console::Error::WriteLine(message);
        }

#if DEBUG
        Debug::Break();
#endif
        assert(false);
    }

    template<typename T>
    void ArgumentNotNull(T * argument, const char * message = nullptr)
    {
        Assert(argument != nullptr, message);
    }

    template<typename T>
    void ArgumentInRange(T argument, T min, T max, const char * message = nullptr)
    {
        Assert(argument >= min && argument <= max, message);
    }
};
