#pragma once

extern "C"
{
    #include "../common.h"
}

namespace Path
{
    utf8 * Append(utf8 * buffer, size_t bufferSize, const utf8 * src);
    utf8 * GetDirectory(utf8 * buffer, size_t bufferSize, const utf8 * path);
    const utf8 * GetFileName(const utf8 * path);
    utf8 * GetFileNameWithoutExtension(utf8 * buffer, size_t bufferSize, const utf8 * path);
    const utf8 * GetExtension(const utf8 * path);
    utf8 * GetAbsolute(utf8 * buffer, size_t bufferSize, const utf8 * relativePath);
    bool Equals(const utf8 * a, const utf8 * b);
}
