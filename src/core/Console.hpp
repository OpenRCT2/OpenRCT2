#pragma once

extern "C"
{
    #include "../common.h"
}

namespace Console
{
    void Write(const utf8 * str)
    {
        fputs(str, stdout);
    }

    void WriteSpace(size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            fputc(' ', stdout);
        }
    }

    void WriteLine()
    {
        puts("");
    }

    void WriteLine(const utf8 * str)
    {
        puts(str);
    }
}
