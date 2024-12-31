/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"

#include <cstdarg>

namespace OpenRCT2::Console
{
    void Write(char c);
    void Write(const utf8* str);
    void WriteSpace(size_t count);
    void WriteFormat(const utf8* format, ...);
    void WriteLine();
    void WriteLine(const utf8* format, ...);

    namespace Error
    {
        void Write(char c);
        void Write(const utf8* str);
        void WriteFormat(const utf8* format, ...);
        void WriteLine();
        void WriteLine(const utf8* format, ...);
        void WriteLine_VA(const utf8* format, va_list args);
    } // namespace Error
} // namespace OpenRCT2::Console
