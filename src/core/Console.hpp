#pragma once

extern "C"
{
    #include "../common.h"
}

namespace Console
{
    void Write(char c);
    void Write(const utf8 * str);
    void WriteSpace(size_t count);
    void WriteFormat(const utf8 * format, ...);
    void WriteLine();
    void WriteLine(const utf8 * str);

    namespace Error
    {
        void Write(char c);
        void Write(const utf8 * str);
        void WriteFormat(const utf8 * format, ...);
        void WriteLine();
        void WriteLine(const utf8 * str);
    }
}
