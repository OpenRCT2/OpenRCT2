#pragma once

extern "C"
{
    #include "../common.h"
}

namespace Path
{
    utf8 * Append(utf8 * buffer, size_t bufferSize, const utf8 * src);
    utf8 * GetAbsolute(utf8 *buffer, size_t bufferSize, const utf8 * relativePath);
}
