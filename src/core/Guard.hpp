#pragma once

/**
 * Utility methods for asserting function parameters.
 */
namespace Guard
{
    void Assert(bool expression, const char * message = nullptr);

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
