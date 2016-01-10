#pragma once

extern "C"
{
    #include "../common.h"
}

namespace String
{
    bool   Equals(const utf8 * a, const utf8 * b, bool ignoreCase = false);
    bool   StartsWith(const utf8 * str, const utf8 * match, bool ignoreCase = false);
    size_t LengthOf(const utf8 * str);
    size_t SizeOf(const utf8 * str);
    utf8 * Set(utf8 * buffer, size_t bufferSize, const utf8 * src);
    utf8 * Set(utf8 * buffer, size_t bufferSize, const utf8 * src, size_t srcSize);
    utf8 * Append(utf8 * buffer, size_t bufferSize, const utf8 * src);
    utf8 * Format(utf8 * buffer, size_t bufferSize, const utf8 * format, ...);
    utf8 * AppendFormat(utf8 * buffer, size_t bufferSize, const utf8 * format, ...);
    utf8 * Duplicate(const utf8 * src);
}
