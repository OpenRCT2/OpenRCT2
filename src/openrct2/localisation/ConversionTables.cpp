/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ConversionTables.h"

#include "FormatCodes.h"

#include <cstdlib>
#include <iterator>

struct encoding_convert_entry
{
    uint16_t code;
    uint32_t unicode;
};

extern const encoding_convert_entry RCT2ToUnicodeTable[];

const encoding_convert_entry RCT2ToUnicodeTable[] = {
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
    const encoding_convert_entry* entry = static_cast<const encoding_convert_entry*>(pEntry);
    if (key < entry->code)
        return -1;
    if (key > entry->code)
        return 1;
    return 0;
}

wchar_t EncodingConvertRCT2ToUnicode(wchar_t rct2str)
{
    encoding_convert_entry* entry = static_cast<encoding_convert_entry*>(std::bsearch(
        &rct2str, RCT2ToUnicodeTable, std::size(RCT2ToUnicodeTable), sizeof(encoding_convert_entry), EncodingSearchCompare));
    if (entry == nullptr)
        return rct2str;
    return entry->unicode;
}
