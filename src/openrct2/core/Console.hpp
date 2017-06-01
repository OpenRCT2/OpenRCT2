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

extern "C"
{
    #include "../common.h"
}

#include <cstdarg>

namespace Console
{
    void Write(char c);
    void Write(const utf8 * str);
    void WriteSpace(size_t count);
    void WriteFormat(const utf8 * format, ...);
    void WriteLine();
    void WriteLine(const utf8 * format, ...);

    namespace Error
    {
        void Write(char c);
        void Write(const utf8 * str);
        void WriteFormat(const utf8 * format, ...);
        void WriteLine();
        void WriteLine(const utf8 * format, ...);
        void WriteLine_VA(const utf8 * format, va_list args);
    }
}
