/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ConversionTables.h"

#include "../core/Util.hpp"
#include "FormatCodes.h"

#include <cstdlib>

// clang-format off
const encoding_convert_entry RCT2ToUnicodeTable[] =
{
    { 1, FORMAT_MOVE_X },
    { 2, FORMAT_ADJUST_PALETTE },
    { 5, FORMAT_NEWLINE },
    { 6, FORMAT_NEWLINE_SMALLER },
    { 7, FORMAT_TINYFONT },
    { 8, FORMAT_BIGFONT },
    { 9, FORMAT_MEDIUMFONT },
    { 10, FORMAT_SMALLFONT },
    { 11, FORMAT_OUTLINE },
    { 12, FORMAT_OUTLINE_OFF },
    { 13, FORMAT_WINDOW_COLOUR_1 },
    { 14, FORMAT_WINDOW_COLOUR_2 },
    { 15, FORMAT_WINDOW_COLOUR_3 },
    { 17, FORMAT_NEWLINE_X_Y },
    { 23, FORMAT_INLINE_SPRITE },
    { 123, FORMAT_COMMA32 },
    { 124, FORMAT_INT32 },
    { 125, FORMAT_COMMA2DP32 },
    { 126, FORMAT_COMMA16 },
    { 127, FORMAT_UINT16 },
    { 128, FORMAT_CURRENCY2DP },
    { 129, FORMAT_CURRENCY },
    { 130, FORMAT_STRINGID },
    { 131, FORMAT_STRINGID2 },
    { 132, FORMAT_STRING },
    { 133, FORMAT_MONTHYEAR },
    { 134, FORMAT_MONTH },
    { 135, FORMAT_VELOCITY },
    { 136, FORMAT_POP16 },
    { 137, FORMAT_PUSH16 },
    { 138, FORMAT_DURATION },
    { 139, FORMAT_REALTIME },
    { 140, FORMAT_LENGTH },
    { 141, FORMAT_SPRITE },
    { 142, FORMAT_BLACK },
    { 143, FORMAT_GREY },
    { 144, FORMAT_WHITE },
    { 145, FORMAT_RED },
    { 146, FORMAT_GREEN },
    { 147, FORMAT_YELLOW },
    { 148, FORMAT_TOPAZ },
    { 149, FORMAT_CELADON },
    { 150, FORMAT_BABYBLUE },
    { 151, FORMAT_PALELAVENDER },
    { 152, FORMAT_PALEGOLD },
    { 153, FORMAT_LIGHTPINK },
    { 154, FORMAT_PEARLAQUA },
    { 155, FORMAT_PALESILVER },
    { RCT2_A_OGONEK_UC, UnicodeChar::a_ogonek_uc },
    { 160, UnicodeChar::up },
    { RCT2_C_ACUTE_UC, UnicodeChar::c_acute_uc },
    { RCT2_E_OGONEK_UC, UnicodeChar::e_ogonek_uc },
    { RCT2_L_STROKE_UC, UnicodeChar::l_stroke_uc },
    { 170, UnicodeChar::down },
    { 172, UnicodeChar::tick },
    { 173, UnicodeChar::cross },
    { 175, UnicodeChar::right },
    { 177, UnicodeChar::railway },
    { 180, UnicodeChar::quote_open },
    { 181, UnicodeChar::euro },
    { 182, UnicodeChar::road },
    { 183, UnicodeChar::air },
    { 184, UnicodeChar::water },
    { 185, UnicodeChar::superscript_minus_one},
    { 186, UnicodeChar::bullet },
    { 188, UnicodeChar::small_up },
    { 189, UnicodeChar::small_down },
    { 190, UnicodeChar::left },
    { RCT2_N_ACUTE_UC, UnicodeChar::n_acute_uc },
    { RCT2_S_ACUTE_UC, UnicodeChar::s_acute_uc },
    { RCT2_Z_ACUTE_UC, UnicodeChar::z_acute_uc },
    { RCT2_Z_DOT_UC, UnicodeChar::z_dot_uc },
    { RCT2_A_OGONEK, UnicodeChar::a_ogonek },
    { RCT2_C_ACUTE, UnicodeChar::c_acute },
    { RCT2_E_OGONEK, UnicodeChar::e_ogonek },
    { RCT2_N_ACUTE, UnicodeChar::n_acute },
    { RCT2_L_STROKE, UnicodeChar::l_stroke },
    { RCT2_S_ACUTE, UnicodeChar::s_acute },
    { RCT2_Z_DOT, UnicodeChar::z_dot },
    { RCT2_Z_ACUTE, UnicodeChar::z_acute },
};

static int32_t encoding_search_compare(const void *pKey, const void *pEntry)
{
    uint16_t key = *((uint16_t*)pKey);
    encoding_convert_entry *entry = (encoding_convert_entry*)pEntry;
    if (key < entry->code) return -1;
    if (key > entry->code) return 1;
    return 0;
}

static wchar_t encoding_convert_x_to_unicode(wchar_t code, const encoding_convert_entry *table, size_t count)
{
    encoding_convert_entry * entry = (encoding_convert_entry *)std::bsearch(&code, table, count, sizeof(encoding_convert_entry), encoding_search_compare);
    if (entry == nullptr) return code;
    else return entry->unicode;
}

wchar_t encoding_convert_rct2_to_unicode(wchar_t rct2str)
{
    return encoding_convert_x_to_unicode(rct2str, RCT2ToUnicodeTable, Util::CountOf(RCT2ToUnicodeTable));
}

uint32_t encoding_convert_unicode_to_rct2(uint32_t unicode)
{
    // Can't do a binary search as it's sorted by RCT2 code, not unicode
    for (const auto& entry : RCT2ToUnicodeTable)
    {
        if (entry.unicode == unicode)
        {
            return entry.code;
        }
    }
    return unicode;
}
