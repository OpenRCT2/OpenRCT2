/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CSStringConverter.h"

#include "../core/String.hpp"
#include "../core/UnicodeChar.h"
#include "../localisation/FormatCodes.h"
#include "../localisation/Language.h"
#include "../rct12/CSChar.h"

#include <cstdlib>
#include <iterator>
#include <limits>
#include <stdexcept>

using namespace OpenRCT2;

struct EncodingConvertEntry
{
    uint16_t code;
    uint32_t unicode;
};

extern const EncodingConvertEntry RCT2ToUnicodeTable[];

const EncodingConvertEntry RCT2ToUnicodeTable[] = {
    // { 1, FORMAT_MOVE_X },
    // { 2, FORMAT_ADJUST_PALETTE },
    // { 5, FORMAT_NEWLINE },
    // { 6, FORMAT_NEWLINE_SMALLER },
    // { 7, FORMAT_TINYFONT },
    // { 8, FORMAT_BIGFONT },
    // { 9, FORMAT_MEDIUMFONT },
    // { 10, FORMAT_SMALLFONT },
    // { 11, FORMAT_OUTLINE },
    // { 12, FORMAT_OUTLINE_OFF },
    // { 13, FORMAT_WINDOW_COLOUR_1 },
    // { 14, FORMAT_WINDOW_COLOUR_2 },
    // { 15, FORMAT_WINDOW_COLOUR_3 },
    // { 17, FORMAT_NEWLINE_X_Y },
    // { 23, FORMAT_INLINE_SPRITE },
    // { 123, FORMAT_COMMA32 },
    // { 124, FORMAT_INT32 },
    // { 125, FORMAT_COMMA2DP32 },
    // { 126, FORMAT_COMMA16 },
    // { 127, FORMAT_UINT16 },
    // { 128, FORMAT_CURRENCY2DP },
    // { 129, FORMAT_CURRENCY },
    // { 130, FORMAT_STRINGID },
    // { 131, FORMAT_STRINGID2 },
    // { 132, FORMAT_STRING },
    // { 133, FORMAT_MONTHYEAR },
    // { 134, FORMAT_MONTH },
    // { 135, FORMAT_VELOCITY },
    // { 136, FORMAT_POP16 },
    // { 137, FORMAT_PUSH16 },
    // { 138, FORMAT_DURATION },
    // { 139, FORMAT_REALTIME },
    // { 140, FORMAT_LENGTH },
    // { 141, FORMAT_SPRITE },
    // { 142, FORMAT_BLACK },
    // { 143, FORMAT_GREY },
    // { 144, FORMAT_WHITE },
    // { 145, FORMAT_RED },
    // { 146, FORMAT_GREEN },
    // { 147, FORMAT_YELLOW },
    // { 148, FORMAT_TOPAZ },
    // { 149, FORMAT_CELADON },
    // { 150, FORMAT_BABYBLUE },
    // { 151, FORMAT_PALELAVENDER },
    // { 152, FORMAT_PALEGOLD },
    // { 153, FORMAT_LIGHTPINK },
    // { 154, FORMAT_PEARLAQUA },
    // { 155, FORMAT_PALESILVER },
    { CSChar::a_ogonek_uc, UnicodeChar::a_ogonek_uc },
    { CSChar::up, UnicodeChar::up },
    { CSChar::c_acute_uc, UnicodeChar::c_acute_uc },
    { CSChar::e_ogonek_uc, UnicodeChar::e_ogonek_uc },
    { CSChar::l_stroke_uc, UnicodeChar::l_stroke_uc },
    { CSChar::down, UnicodeChar::down },
    { CSChar::tick, UnicodeChar::tick },
    { CSChar::cross, UnicodeChar::cross },
    { CSChar::right, UnicodeChar::right },
    { CSChar::railway, UnicodeChar::railway },
    { CSChar::quote_open, UnicodeChar::quote_open },
    { CSChar::euro, UnicodeChar::euro },
    { CSChar::road, UnicodeChar::road },
    { CSChar::air, UnicodeChar::air },
    { CSChar::water, UnicodeChar::water },
    { CSChar::superscript_minus_one, UnicodeChar::superscript_minus_one },
    { CSChar::bullet, UnicodeChar::bullet },
    { CSChar::small_up, UnicodeChar::small_up },
    { CSChar::small_down, UnicodeChar::small_down },
    { CSChar::left, UnicodeChar::left },
    { CSChar::n_acute_uc, UnicodeChar::n_acute_uc },
    { CSChar::s_acute_uc, UnicodeChar::s_acute_uc },
    { CSChar::z_acute_uc, UnicodeChar::z_acute_uc },
    { CSChar::z_dot_uc, UnicodeChar::z_dot_uc },
    { CSChar::a_ogonek, UnicodeChar::a_ogonek },
    { CSChar::c_acute, UnicodeChar::c_acute },
    { CSChar::e_ogonek, UnicodeChar::e_ogonek },
    { CSChar::n_acute, UnicodeChar::n_acute },
    { CSChar::l_stroke, UnicodeChar::l_stroke },
    { CSChar::s_acute, UnicodeChar::s_acute },
    { CSChar::z_dot, UnicodeChar::z_dot },
    { CSChar::z_acute, UnicodeChar::z_acute },
};

static int32_t EncodingSearchCompare(const void* pKey, const void* pEntry)
{
    const uint16_t key = *reinterpret_cast<const uint16_t*>(pKey);
    const EncodingConvertEntry* entry = static_cast<const EncodingConvertEntry*>(pEntry);
    if (key < entry->code)
        return -1;
    if (key > entry->code)
        return 1;
    return 0;
}

wchar_t EncodingConvertRCT2ToUnicode(wchar_t rct2str)
{
    EncodingConvertEntry* entry = static_cast<EncodingConvertEntry*>(std::bsearch(
        &rct2str, RCT2ToUnicodeTable, std::size(RCT2ToUnicodeTable), sizeof(EncodingConvertEntry), EncodingSearchCompare));
    if (entry == nullptr)
        return rct2str;
    return entry->unicode;
}

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
            return OpenRCT2::CodePage::CP_932;
        case RCT2LanguageId::ChineseSimplified:
            return OpenRCT2::CodePage::CP_936;
        case RCT2LanguageId::Korean:
            return OpenRCT2::CodePage::CP_949;
        case RCT2LanguageId::ChineseTraditional:
            return OpenRCT2::CodePage::CP_950;
        default:
            return OpenRCT2::CodePage::CP_1252;
    }
}

template<typename TConvertFunc>
static std::string DecodeConvertWithTable(std::string_view src, TConvertFunc func)
{
    auto decoded = DecodeToWideChar(src);
    std::wstring u16;
    u16.reserve(decoded.size());
    for (auto cc : decoded)
    {
        u16.push_back(func(cc));
    }
    return String::toUtf8(u16);
}

std::string RCT2StringToUTF8(std::string_view src, RCT2LanguageId languageId)
{
    auto codePage = GetCodePageForRCT2Language(languageId);
    if (codePage == OpenRCT2::CodePage::CP_1252)
    {
        // The code page used by RCT2 was not quite 1252 as some codes were used for Polish characters.
        return DecodeConvertWithTable(src, EncodingConvertRCT2ToUnicode);
    }

    auto decoded = DecodeToMultiByte(src);
    return String::convertToUtf8(decoded, codePage);
}
