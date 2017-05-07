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

extern "C"
{
    #include "../platform/platform.h"
}

#include <android/log.h>
#include "Console.hpp"

namespace Console
{
    void Write(char c)
    {
        fputc(c, stdout);
    }

    void Write(const utf8 * str)
    {
        fputs(str, stdout);
    }

    void WriteSpace(size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            Write(' ');
        }
    }

    void WriteFormat(const utf8 * format, ...)
    {
        va_list args;

        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_INFO, "OpenRCT2", format, args);
        va_end(args);
    }

    void WriteLine()
    {
        __android_log_write(ANDROID_LOG_INFO, "OpenRCT2", "");
    }

    void WriteLine(const utf8 * format, ...)
    {
        va_list args;

        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_INFO, "OpenRCT2", format, args);
        va_end(args);
    }

    namespace Error
    {
        void Write(char c)
        {
            fputc(c, stderr);
        }

        void Write(const utf8 * str)
        {
            __android_log_write(ANDROID_LOG_ERROR, "OpenRCT2", str);
        }

        void WriteFormat(const utf8 * format, ...)
        {
            va_list args;

            va_start(args, format);
            __android_log_vprint(ANDROID_LOG_ERROR, "OpenRCT2", format, args);
            va_end(args);
        }

        void WriteLine()
        {
            __android_log_write(ANDROID_LOG_ERROR, "OpenRCT2", "");
        }

        void WriteLine(const utf8 * format, ...)
        {
            va_list args;
            va_start(args, format);
            WriteLine_VA(format, args);
            va_end(args);
        }

        void WriteLine_VA(const utf8 * format, va_list args)
        {
            __android_log_vprint(ANDROID_LOG_ERROR, "OpenRCT2", format, args);
        }
    }
}
