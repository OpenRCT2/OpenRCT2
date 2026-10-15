/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <string>

using utf8 = char;
using utf8string = utf8*;
using const_utf8string = const utf8*;
using u8string = std::basic_string<utf8>;
using u8string_view = std::basic_string_view<utf8>;

using codepoint_t = uint32_t;

namespace OpenRCT2
{
    enum class CodePage : int32_t
    {
        cp932 = 932,   // ANSI/OEM Japanese; Japanese (Shift-JIS)
        cp936 = 936,   // ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
        cp949 = 949,   // ANSI/OEM Korean (Unified Hangul Code)
        cp950 = 950,   // ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)
        cp1252 = 1252, // ANSI Latin 1; Western European (Windows)
        utf8 = 65001,  // Unicode (UTF-8)
    };
}
