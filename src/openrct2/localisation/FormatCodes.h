/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <string_view>
namespace OpenRCT2
{
    enum class FormatToken
    {
        Unknown,
        Literal,
        Escaped,

        Newline,
        NewlineSmall,

        // With parameters
        Move,
        InlineSprite,

        // With arguments
        Comma32,
        Int32,
        Comma1dp16,
        Comma2dp32,
        Comma16,
        UInt16,
        Currency2dp,
        Currency,
        StringById,
        String,
        MonthYear,
        Month,
        Velocity,
        DurationShort,
        DurationLong,
        Length,
        Sprite,
        Pop16,
        Push16,

        // Colours
        ColourWindow1,
        ColourWindow2,
        ColourWindow3,
        ColourBlack,
        ColourGrey,
        ColourWhite,
        ColourRed,
        ColourGreen,
        ColourYellow,
        ColourTopaz,
        ColourCeladon,
        ColourBabyBlue,
        ColourPaleLavender,
        ColourPaleGold,
        ColourLightPink,
        ColourPearlAqua,
        ColourPaleSilver,

        // Fonts
        FontTiny,
        FontSmall,
        FontMedium,

        OutlineEnable,
        OutlineDisable,
    };

    std::string_view GetFormatTokenStringWithBraces(FormatToken token);
    FormatToken FormatTokenFromString(std::string_view token);
    std::string_view FormatTokenToString(FormatToken token, bool withBraces = false);
    bool FormatTokenTakesArgument(FormatToken token);
    bool FormatTokenIsColour(FormatToken token);
    size_t FormatTokenGetTextColourIndex(FormatToken token);
    FormatToken FormatTokenFromTextColour(size_t textColour);

    constexpr uint8_t CS_SPRITE_FONT_OFFSET = 32;

    namespace CSChar
    {
        constexpr char32_t quote_close = 0x22;

        constexpr char32_t a_ogonek_uc = 0x9F;
        constexpr char32_t up = 0xA0;

        constexpr char32_t c_acute_uc = 0xA2;
        constexpr char32_t e_ogonek_uc = 0xA6;
        constexpr char32_t l_stroke_uc = 0xA7;

        constexpr char32_t down = 0xAA;
        constexpr char32_t tick = 0xAC;
        constexpr char32_t cross = 0xAD;
        constexpr char32_t right = 0xAF;
        constexpr char32_t railway = 0xB1;
        constexpr char32_t quote_open = 0xB4;
        constexpr char32_t euro = 0xB5;
        constexpr char32_t road = 0xB6;
        constexpr char32_t air = 0xB7;
        constexpr char32_t water = 0xB8;
        constexpr char32_t superscript_minus_one = 0xB9;
        constexpr char32_t bullet = 0xBA;
        constexpr char32_t small_up = 0xBC;
        constexpr char32_t small_down = 0xBD;
        constexpr char32_t left = 0xBE;

        constexpr char32_t n_acute_uc = 0xC6;
        constexpr char32_t s_acute_uc = 0xD0;
        constexpr char32_t z_acute_uc = 0xD7;
        constexpr char32_t z_dot_uc = 0xD8;
        constexpr char32_t a_ogonek = 0xDD;
        constexpr char32_t c_acute = 0xDE;
        constexpr char32_t e_ogonek = 0xE6;
        constexpr char32_t n_acute = 0xF0;
        constexpr char32_t o_circumflex = 0xF4;
        constexpr char32_t l_stroke = 0xF7;
        constexpr char32_t s_acute = 0xF8;
        constexpr char32_t z_acute = 0xFE;

        constexpr char32_t z_dot = 0xFD;
    } // namespace CSChar

    namespace UnicodeChar
    {
        // Latin alphabet
        constexpr char32_t j = 0x6A;
        constexpr char32_t l = 0x6C;
        constexpr char32_t ae_uc = 0xC6;
        constexpr char32_t o_stroke_uc = 0xD8;
        constexpr char32_t y_acute_uc = 0xDD;
        constexpr char32_t ae = 0xE6;
        constexpr char32_t o_stroke = 0xF8;
        constexpr char32_t y_acute = 0xFD;
        constexpr char32_t a_breve_uc = 0x102;
        constexpr char32_t a_breve = 0x103;
        constexpr char32_t a_ogonek_uc = 0x104;
        constexpr char32_t a_ogonek = 0x105;
        constexpr char32_t c_acute_uc = 0x106;
        constexpr char32_t c_acute = 0x107;
        constexpr char32_t c_circumflex_uc = 0x108;
        constexpr char32_t c_circumflex = 0x109;
        constexpr char32_t c_caron_uc = 0x10C;
        constexpr char32_t c_caron = 0x10D;
        constexpr char32_t d_caron_uc = 0x10E;
        constexpr char32_t d_caron = 0x10F;
        constexpr char32_t e_ogonek_uc = 0x118;
        constexpr char32_t e_ogonek = 0x119;
        constexpr char32_t e_caron_uc = 0x11A;
        constexpr char32_t e_caron = 0x11B;
        constexpr char32_t g_circumflex_uc = 0x11C;
        constexpr char32_t g_circumflex = 0x11D;
        constexpr char32_t g_breve_uc = 0x11E;
        constexpr char32_t g_breve = 0x11F;
        constexpr char32_t h_circumflex_uc = 0x124;
        constexpr char32_t h_circumflex = 0x125;
        constexpr char32_t i_with_dot_uc = 0x130;
        constexpr char32_t i_without_dot = 0x131;
        constexpr char32_t j_circumflex_uc = 0x134;
        constexpr char32_t j_circumflex = 0x135;
        constexpr char32_t l_stroke_uc = 0x141;
        constexpr char32_t l_stroke = 0x142;
        constexpr char32_t n_acute_uc = 0x143;
        constexpr char32_t n_acute = 0x144;
        constexpr char32_t n_caron_uc = 0x147;
        constexpr char32_t n_caron = 0x148;
        constexpr char32_t o_macron = 0x14D;
        constexpr char32_t o_double_acute_uc = 0x150;
        constexpr char32_t o_double_acute = 0x151;
        constexpr char32_t oe_uc = 0x152;
        constexpr char32_t oe = 0x153;
        constexpr char32_t r_caron_uc = 0x158;
        constexpr char32_t r_caron = 0x159;
        constexpr char32_t s_acute_uc = 0x15A;
        constexpr char32_t s_acute = 0x15B;
        constexpr char32_t s_circumflex_uc = 0x15C;
        constexpr char32_t s_circumflex = 0x15D;
        constexpr char32_t s_cedilla_uc = 0x15E;
        constexpr char32_t s_cedilla = 0x15F;
        constexpr char32_t s_caron_uc = 0x160;
        constexpr char32_t s_caron = 0x161;
        constexpr char32_t t_caron_uc = 0x164;
        constexpr char32_t t_caron = 0x165;
        constexpr char32_t u_breve_uc = 0x16C;
        constexpr char32_t u_breve = 0x16D;
        constexpr char32_t u_ring_uc = 0x16E;
        constexpr char32_t u_ring = 0x16F;
        constexpr char32_t u_double_acute_uc = 0x170;
        constexpr char32_t u_double_acute = 0x171;
        constexpr char32_t z_acute_uc = 0x179;
        constexpr char32_t z_acute = 0x17A;
        constexpr char32_t z_dot_uc = 0x17B;
        constexpr char32_t z_dot = 0x17C;
        constexpr char32_t z_caron_uc = 0x17D;
        constexpr char32_t z_caron = 0x17E;
        constexpr char32_t f_with_hook_uc = 0x191;
        constexpr char32_t s_comma_uc = 0x218;
        constexpr char32_t s_comma = 0x219;
        constexpr char32_t t_comma_uc = 0x21A;
        constexpr char32_t t_comma = 0x21B;
        constexpr char32_t sharp_s_uc = 0x1E9E;

        // Cyrillic alphabet
        constexpr char32_t cyrillic_io_uc = 0x401;
        constexpr char32_t cyrillic_a_uc = 0x410;
        constexpr char32_t cyrillic_be_uc = 0x411;
        constexpr char32_t cyrillic_ve_uc = 0x412;
        constexpr char32_t cyrillic_ghe_uc = 0x413;
        constexpr char32_t cyrillic_de_uc = 0x414;
        constexpr char32_t cyrillic_ie_uc = 0x415;
        constexpr char32_t cyrillic_zhe_uc = 0x416;
        constexpr char32_t cyrillic_ze_uc = 0x417;
        constexpr char32_t cyrillic_i_uc = 0x418;
        constexpr char32_t cyrillic_short_i_uc = 0x419;
        constexpr char32_t cyrillic_ka_uc = 0x41A;
        constexpr char32_t cyrillic_el_uc = 0x41B;
        constexpr char32_t cyrillic_em_uc = 0x41C;
        constexpr char32_t cyrillic_en_uc = 0x41D;
        constexpr char32_t cyrillic_o_uc = 0x41E;
        constexpr char32_t cyrillic_pe_uc = 0x41F;
        constexpr char32_t cyrillic_er_uc = 0x420;
        constexpr char32_t cyrillic_es_uc = 0x421;
        constexpr char32_t cyrillic_te_uc = 0x422;
        constexpr char32_t cyrillic_u_uc = 0x423;
        constexpr char32_t cyrillic_ef_uc = 0x424;
        constexpr char32_t cyrillic_ha_uc = 0x425;
        constexpr char32_t cyrillic_tse_uc = 0x426;
        constexpr char32_t cyrillic_che_uc = 0x427;
        constexpr char32_t cyrillic_sha_uc = 0x428;
        constexpr char32_t cyrillic_shcha_uc = 0x429;
        constexpr char32_t cyrillic_hard_sign_uc = 0x42A;
        constexpr char32_t cyrillic_yeru_uc = 0x42B;
        constexpr char32_t cyrillic_soft_sign_uc = 0x42C;
        constexpr char32_t cyrillic_e_uc = 0x42D;
        constexpr char32_t cyrillic_yu_uc = 0x42E;
        constexpr char32_t cyrillic_ya_uc = 0x42F;

        constexpr char32_t cyrillic_a = 0x430;
        constexpr char32_t cyrillic_be = 0x431;
        constexpr char32_t cyrillic_ve = 0x432;
        constexpr char32_t cyrillic_ghe = 0x433;
        constexpr char32_t cyrillic_de = 0x434;
        constexpr char32_t cyrillic_ie = 0x435;
        constexpr char32_t cyrillic_zhe = 0x436;
        constexpr char32_t cyrillic_ze = 0x437;
        constexpr char32_t cyrillic_i = 0x438;
        constexpr char32_t cyrillic_short_i = 0x439;
        constexpr char32_t cyrillic_ka = 0x43A;
        constexpr char32_t cyrillic_el = 0x43B;
        constexpr char32_t cyrillic_em = 0x43C;
        constexpr char32_t cyrillic_en = 0x43D;
        constexpr char32_t cyrillic_o = 0x43E;
        constexpr char32_t cyrillic_pe = 0x43F;
        constexpr char32_t cyrillic_er = 0x440;
        constexpr char32_t cyrillic_es = 0x441;
        constexpr char32_t cyrillic_te = 0x442;
        constexpr char32_t cyrillic_u = 0x443;
        constexpr char32_t cyrillic_ef = 0x444;
        constexpr char32_t cyrillic_ha = 0x445;
        constexpr char32_t cyrillic_tse = 0x446;
        constexpr char32_t cyrillic_che = 0x447;
        constexpr char32_t cyrillic_sha = 0x448;
        constexpr char32_t cyrillic_shcha = 0x449;
        constexpr char32_t cyrillic_hard_sign = 0x44A;
        constexpr char32_t cyrillic_yeru = 0x44B;
        constexpr char32_t cyrillic_soft_sign = 0x44C;
        constexpr char32_t cyrillic_e = 0x44D;
        constexpr char32_t cyrillic_yu = 0x44E;
        constexpr char32_t cyrillic_ya = 0x44F;

        constexpr char32_t cyrillic_io = 0x451;

        // Punctuation
        constexpr char32_t left_brace = 0x7B;
        constexpr char32_t vertical_bar = 0x7C;
        constexpr char32_t right_brace = 0x7D;
        constexpr char32_t tilde = 0x7E;
        constexpr char32_t non_breaking_space = 0xA0;
        constexpr char32_t leftguillemet = 0xAB;
        constexpr char32_t rightguillemet = 0xBB;
        constexpr char32_t interpunct = 0xB7;
        constexpr char32_t multiplication_sign = 0xD7;
        constexpr char32_t en_dash = 0x2013;
        constexpr char32_t em_dash = 0x2014;
        constexpr char32_t single_quote_open = 0x2018;
        constexpr char32_t single_quote_end = 0x2019;
        constexpr char32_t single_german_quote_open = 0x201A;
        constexpr char32_t german_quote_open = 0x201E;
        constexpr char32_t bullet = 0x2022;
        constexpr char32_t ellipsis = 0x2026;
        constexpr char32_t narrow_non_breaking_space = 0x202F;
        constexpr char32_t quote_open = 0x201C;
        constexpr char32_t quote_close = 0x201D;

        // Currency
        constexpr char32_t guilder = 0x192;
        constexpr char32_t euro = 0x20AC;
        constexpr char32_t rouble = 0x20BD;

        // Dingbats
        constexpr char32_t up = 0x25B2;
        constexpr char32_t small_up = 0x25B4;
        constexpr char32_t right = 0x25B6;
        constexpr char32_t down = 0x25BC;
        constexpr char32_t small_down = 0x25BE;
        constexpr char32_t left = 0x25C0;
        constexpr char32_t air = 0x2601;
        constexpr char32_t tick = 0x2713;
        constexpr char32_t plus = 0x2795;
        constexpr char32_t minus = 0x2796;

        // Emoji
        constexpr char32_t cross = 0x274C;
        constexpr char32_t variation_selector = 0xFE0F;
        constexpr char32_t water = 0x1F30A;
        constexpr char32_t eye = 0x1F441;
        constexpr char32_t road = 0x1F6E3;
        constexpr char32_t railway = 0x1F6E4;

        // Misc
        constexpr char32_t superscript_minus_one = 0x207B;

    }; // namespace UnicodeChar
} // namespace OpenRCT2
