/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Console.hpp"

#include "../Context.h"
#include "../platform/Platform.h"

#include <cstdio>
#include <string>

namespace Console
{
    void Write(char c)
    {
        fputc(c, stdout);
    }

    void Write(const utf8* str)
    {
        fputs(str, stdout);
    }

    void WriteSpace(size_t count)
    {
        std::string sz(count, ' ');
        Write(sz.c_str());
    }

    void WriteFormat(const utf8* format, ...)
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

    void WriteLine(const utf8* format, ...)
    {
        va_list args;
        va_start(args, format);

        char buffer[4096];
        std::vsnprintf(buffer, sizeof(buffer), format, args);
        auto ctx = OpenRCT2::GetContext();
        if (ctx != nullptr)
            ctx->WriteLine(buffer);
        else
            std::printf("%s\n", buffer);

        va_end(args);
    }

    namespace Error
    {
        void Write(char c)
        {
            fputc(c, stderr);
        }

        void Write(const utf8* str)
        {
            fputs(str, stderr);
        }

        void WriteFormat(const utf8* format, ...)
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

        void WriteLine(const utf8* format, ...)
        {
            va_list args;
            va_start(args, format);
            WriteLine_VA(format, args);
            va_end(args);
        }

        void WriteLine_VA(const utf8* format, va_list args)
        {
            char buffer[4096];
            std::vsnprintf(buffer, sizeof(buffer), format, args);
            auto ctx = OpenRCT2::GetContext();
            if (ctx != nullptr)
                ctx->WriteErrorLine(buffer);
            else
                std::printf("%s\n", buffer);
        }
    } // namespace Error
} // namespace Console
