/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

enum class RCT2LanguageId : uint8_t;

wchar_t EncodingConvertRCT2ToUnicode(wchar_t rct2str);

std::string RCT2StringToUTF8(std::string_view src, RCT2LanguageId languageId);
