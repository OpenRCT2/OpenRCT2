/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/String.hpp"
#include "ConversionTables.h"
#include "Language.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

/**
 * Decodes an RCT2 string to a wide char string still in the original code page.
 * An RCT2 string is a multi-byte string where every two-byte code point is preceded with a byte value of 255.
 */
static std::wstring DecodeToWideChar(std::string_view src)
{
    std::wstring decoded;
    decoded.reserve(src.size());
    for (auto it = src.begin(); it != src.end();)
    {
        uint8_t c = *it++;
        if (c == 255)
        {
            // Push next two characters
            uint8_t a = 0;
            uint8_t b = 0;
            if (it != src.end())
            {
                a = *it++;
                if (it != src.end())
                {
                    b = *it++;
                }
                else
                {
                    // 2nd byte for double byte character is missing
                    break;
                }
            }
            else
            {
                // 1st byte for double byte character is missing
                break;
            }

            wchar_t cp = (a << 8) | b;
            decoded.push_back(cp);
        }
        else
        {
            // Push character
            decoded.push_back(c);
        }
    }
    return decoded;
}

static std::string DecodeToMultiByte(std::string_view src)
{
    auto wide = DecodeToWideChar(src);
    std::string result;
    result.reserve(wide.size());
    for (auto cc : wide)
    {
        if (cc <= 255)
        {
            result.push_back(cc);
        }
        else
        {
            result.push_back((cc >> 8) & 0xFF);
            result.push_back(cc & 0xFF);
        }
    }
    return result;
}

static int32_t GetCodePageForRCT2Language(RCT2LanguageId languageId)
{
    switch (languageId)
    {
        case RCT2LanguageId::Japanese:
            return CODE_PAGE::CP_932;
        case RCT2LanguageId::ChineseSimplified:
            return CODE_PAGE::CP_936;
        case RCT2LanguageId::Korean:
            return CODE_PAGE::CP_949;
        case RCT2LanguageId::ChineseTraditional:
            return CODE_PAGE::CP_950;
        default:
            return CODE_PAGE::CP_1252;
    }
}

template<typename TConvertFunc> static std::string DecodeConvertWithTable(std::string_view src, TConvertFunc func)
{
    auto decoded = DecodeToWideChar(src);
    std::wstring u16;
    u16.reserve(decoded.size());
    for (auto cc : decoded)
    {
        u16.push_back(func(cc));
    }
    return String::ToUtf8(u16);
}

std::string rct2_to_utf8(std::string_view src, RCT2LanguageId languageId)
{
    auto codePage = GetCodePageForRCT2Language(languageId);
    if (codePage == CODE_PAGE::CP_1252)
    {
        // The code page used by RCT2 was not quite 1252 as some codes were used for Polish characters.
        return DecodeConvertWithTable(src, encoding_convert_rct2_to_unicode);
    }

    auto decoded = DecodeToMultiByte(src);
    return String::ConvertToUtf8(decoded, codePage);
}
