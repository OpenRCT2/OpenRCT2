/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <limits>
#include <stdexcept>
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "ConversionTables.h"
#include "Language.h"

/**
 * Decodes an RCT2 string to a wide char string still in the original code page.
 * An RCT2 string is a multi-byte string where every two-byte code point is preceeded with a byte value of 255.
 */
static std::wstring DecodeToWideChar(const std::string_view& src)
{
    std::wstring decoded;
    decoded.reserve(src.size());
    for (auto it = src.begin(); it != src.end(); )
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

static std::string DecodeToMultiByte(const std::string_view& src)
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

/**
 * Encodes a UTF-8 string as an RCT2 string.
 */
static std::string Encode(const std::string_view& src)
{
    std::string dst;
    const utf8 * ch = src.data();
    int32_t codepoint;
    while ((codepoint = utf8_get_next(ch, &ch)) != 0)
    {
        codepoint = encoding_convert_unicode_to_rct2(codepoint);
        if (codepoint <= std::numeric_limits<uint8_t>::max())
        {
            dst.push_back(codepoint);
        }
        else if (codepoint <= std::numeric_limits<uint16_t>::max())
        {
            dst.push_back((char)(uint8_t)0xFF);
            dst.push_back((codepoint >> 8) & 0xFF);
            dst.push_back(codepoint & 0xFF);
        }
        else
        {
            // RCT2 strings do not support code points greater than 65535, replace them with '?'
            dst.push_back('?');
        }
    }
    return dst;
}

static int32_t GetCodePageForRCT2Language(RCT2LanguageId languageId)
{
    switch (languageId)
    {
        case RCT2_LANGUAGE_ID_JAPANESE:
            return CODE_PAGE::CP_932;
        case RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED:
            return CODE_PAGE::CP_936;
        case RCT2_LANGUAGE_ID_KOREAN:
            return CODE_PAGE::CP_949;
        case RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL:
            return CODE_PAGE::CP_950;
        default:
            return CODE_PAGE::CP_1252;
    }
}

template<typename TConvertFunc>
static std::string DecodeConvertWithTable(const std::string_view& src, TConvertFunc func)
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

std::string rct2_to_utf8(const std::string_view& src, RCT2LanguageId languageId)
{
    auto codePage = GetCodePageForRCT2Language(languageId);
    if (codePage == CODE_PAGE::CP_1252)
    {
        // The code page used by RCT2 was not quite 1252 as some codes were used for Polish characters.
        return DecodeConvertWithTable(src, encoding_convert_rct2_to_unicode);
    }
    else
    {
        auto decoded = DecodeToMultiByte(src);
        return String::Convert(decoded, codePage, CODE_PAGE::CP_UTF8);
    }
}

std::string utf8_to_rct2(const std::string_view& src)
{
    // NOTE: This is only used for SC6 / SV6 files which don't store the language identifier
    //       because of this, we can only store in RCT2's CP_1252 format. We can preserve some
    //       unicode characters, but only those between 256 and 65535.
    return Encode(src);
}
