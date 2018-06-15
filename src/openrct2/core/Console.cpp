/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cstdio>
#include <string>

#include "Console.hpp"
#include "../platform/platform.h"

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
        std::string sz(count, ' ');
        Write(sz.c_str());
    }

    void WriteFormat(const utf8 * format, ...)
    {
        va_list args;

        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
    }

    void WriteLine()
    {
        puts("");
    }

    void WriteLine(const utf8 * format, ...)
    {
        va_list args;

        va_start(args, format);
        auto formatLn = std::string(format) + "\n";
        vfprintf(stdout, formatLn.c_str(), args);
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
            fputs(str, stderr);
        }

        void WriteFormat(const utf8 * format, ...)
        {
            va_list args;

            va_start(args, format);
            vfprintf(stderr, format, args);
            va_end(args);
        }

        void WriteLine()
        {
            fputs(PLATFORM_NEWLINE, stderr);
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
            auto formatLn = std::string(format) + "\n";
            vfprintf(stdout, formatLn.c_str(), args);
        }
    } // namespace Error
} // namespace Console
