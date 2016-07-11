#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

/**
 * Utility methods for asserting function parameters.
 */
namespace Guard
{
    void Assert(bool expression, const char * message = nullptr);
    void Fail(const char * message = nullptr);

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
