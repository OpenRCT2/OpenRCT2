/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Font.h"

#include "../Diagnostic.h"
#include "../SpriteIds.h"
#include "../core/EnumUtils.hpp"
#include "../core/UTF8.h"
#include "../core/UnicodeChar.h"
#include "../localisation/LocalisationService.h"
#include "../rct12/CSChar.h"
#include "Drawing.h"
#include "ScrollingText.h"
#include "TTF.h"

#include <iterator>
#include <limits>
#include <unordered_map>

using namespace OpenRCT2;

static constexpr int32_t kSpriteFontLineHeight[FontStyleCount] = {
    10,
    10,
    6,
};

static uint8_t _spriteFontCharacterWidths[FontStyleCount][SPR_FONTS_GLYPH_COUNT] = {};

#ifndef DISABLE_TTF
TTFFontSetDescriptor* gCurrentTTFFontSet;
#endif // DISABLE_TTF

constexpr uint8_t kCSSpriteFontOffset = 32;

static const std::unordered_map<char32_t, int32_t> codepointOffsetMap = {
    { UnicodeChar::ae_uc, SPR_FONTS_AE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::o_stroke_uc, SPR_FONTS_O_STROKE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::y_acute_uc, SPR_FONTS_Y_ACUTE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::ae, SPR_FONTS_AE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::o_stroke, SPR_FONTS_O_STROKE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::y_acute, SPR_FONTS_Y_ACUTE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::a_breve_uc, SPR_FONTS_A_BREVE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::a_breve, 226 - kCSSpriteFontOffset }, // Render as â, no visual difference in the RCT font
    { UnicodeChar::a_ogonek_uc, CSChar::a_ogonek_uc - kCSSpriteFontOffset },
    { UnicodeChar::a_ogonek, CSChar::a_ogonek - kCSSpriteFontOffset },
    { UnicodeChar::c_acute_uc, CSChar::c_acute_uc - kCSSpriteFontOffset },
    { UnicodeChar::c_acute, CSChar::c_acute - kCSSpriteFontOffset },
    { UnicodeChar::c_caron_uc, SPR_FONTS_C_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::c_caron, SPR_FONTS_C_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::d_caron_uc, SPR_FONTS_D_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::d_caron, SPR_FONTS_D_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::e_ogonek_uc, CSChar::e_ogonek_uc - kCSSpriteFontOffset },
    { UnicodeChar::e_ogonek, CSChar::e_ogonek - kCSSpriteFontOffset },
    { UnicodeChar::e_caron_uc, SPR_FONTS_E_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::e_caron, SPR_FONTS_E_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::g_breve_uc, SPR_FONTS_G_BREVE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::g_breve, SPR_FONTS_G_BREVE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::i_with_dot_uc, SPR_FONTS_I_WITH_DOT_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::i_without_dot, SPR_FONTS_I_WITHOUT_DOT_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::l_stroke_uc, CSChar::l_stroke_uc - kCSSpriteFontOffset },
    { UnicodeChar::l_stroke, CSChar::l_stroke - kCSSpriteFontOffset },
    { UnicodeChar::n_acute_uc, CSChar::n_acute_uc - kCSSpriteFontOffset },
    { UnicodeChar::n_acute, CSChar::n_acute - kCSSpriteFontOffset },
    { UnicodeChar::n_caron_uc, SPR_FONTS_N_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::n_caron, SPR_FONTS_N_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::o_macron, CSChar::o_circumflex - kCSSpriteFontOffset }, // No visual difference
    { UnicodeChar::o_double_acute_uc, SPR_FONTS_O_DOUBLE_ACUTE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::o_double_acute, SPR_FONTS_O_DOUBLE_ACUTE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::oe_uc, SPR_FONTS_OE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::oe, SPR_FONTS_OE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::r_caron_uc, SPR_FONTS_R_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::r_caron, SPR_FONTS_R_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::s_acute_uc, CSChar::s_acute_uc - kCSSpriteFontOffset },
    { UnicodeChar::s_acute, CSChar::s_acute - kCSSpriteFontOffset },
    { UnicodeChar::s_cedilla_uc, SPR_FONTS_S_CEDILLA_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::s_cedilla, SPR_FONTS_S_CEDILLA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::s_caron_uc, SPR_FONTS_S_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::s_caron, SPR_FONTS_S_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::t_caron_uc, SPR_FONTS_T_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::t_caron, SPR_FONTS_T_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::u_ring_uc, SPR_FONTS_U_RING_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::u_ring, SPR_FONTS_U_RING_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::u_double_acute_uc, SPR_FONTS_U_DOUBLE_ACUTE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::u_double_acute, SPR_FONTS_U_DOUBLE_ACUTE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::w_circumflex_uc, SPR_FONTS_W_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::w_circumflex, SPR_FONTS_W_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::y_circumflex_uc, SPR_FONTS_Y_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::y_circumflex, SPR_FONTS_Y_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::z_acute_uc, CSChar::z_acute_uc - kCSSpriteFontOffset },
    { UnicodeChar::z_acute, CSChar::z_acute - kCSSpriteFontOffset },
    { UnicodeChar::z_dot_uc, CSChar::z_dot_uc - kCSSpriteFontOffset },
    { UnicodeChar::z_dot, CSChar::z_dot - kCSSpriteFontOffset },
    { UnicodeChar::z_caron_uc, SPR_FONTS_Z_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::z_caron, SPR_FONTS_Z_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::f_with_hook_uc, 'F' - kCSSpriteFontOffset },
    { UnicodeChar::s_comma_uc, SPR_FONTS_S_CEDILLA_UPPER - SPR_FONTS_BEGIN }, // No visual difference
    { UnicodeChar::s_comma, SPR_FONTS_S_CEDILLA_LOWER - SPR_FONTS_BEGIN },    // Ditto
    { UnicodeChar::t_comma_uc, SPR_FONTS_T_COMMA_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::t_comma, SPR_FONTS_T_COMMA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::sharp_s_uc, 223 - kCSSpriteFontOffset },
    { UnicodeChar::c_circumflex_uc, SPR_FONTS_C_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::c_circumflex, SPR_FONTS_C_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::g_circumflex_uc, SPR_FONTS_G_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::g_circumflex, SPR_FONTS_G_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::h_circumflex_uc, SPR_FONTS_H_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::h_circumflex, SPR_FONTS_H_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::j_circumflex_uc, SPR_FONTS_J_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::j_circumflex, SPR_FONTS_J_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::s_circumflex_uc, SPR_FONTS_S_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::s_circumflex, SPR_FONTS_S_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::u_breve_uc, SPR_FONTS_U_BREVE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::u_breve, SPR_FONTS_U_BREVE_LOWER - SPR_FONTS_BEGIN },

    // Cyrillic alphabet
    { UnicodeChar::cyrillic_io_uc, 203 - kCSSpriteFontOffset }, // Looks just like Ë
    { UnicodeChar::cyrillic_ukrainian_ie_uc, SPR_FONTS_CYRILLIC_UKRAINIAN_IE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_dze_uc, 'S' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_dotted_i_uc, 'I' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_yi_uc, 207 - kCSSpriteFontOffset }, // Looks just like Ï
    { UnicodeChar::cyrillic_je_uc, 'J' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_a_uc, 'A' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_be_uc, SPR_FONTS_CYRILLIC_BE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ve_uc, 'B' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_ghe_uc, SPR_FONTS_CYRILLIC_GHE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_de_uc, SPR_FONTS_CYRILLIC_DE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ie_uc, 'E' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_zhe_uc, SPR_FONTS_CYRILLIC_ZHE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ze_uc, SPR_FONTS_CYRILLIC_ZE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_i_uc, SPR_FONTS_CYRILLIC_I_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_short_i_uc, SPR_FONTS_CYRILLIC_SHORT_I_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ka_uc, 'K' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_el_uc, SPR_FONTS_CYRILLIC_EL_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_em_uc, 'M' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_en_uc, 'H' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_o_uc, 'O' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_pe_uc, SPR_FONTS_CYRILLIC_PE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_er_uc, 'P' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_es_uc, 'C' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_te_uc, 'T' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_u_uc, SPR_FONTS_CYRILLIC_U_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ef_uc, SPR_FONTS_CYRILLIC_EF_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ha_uc, 'X' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_tse_uc, SPR_FONTS_CYRILLIC_TSE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_che_uc, SPR_FONTS_CYRILLIC_CHE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_sha_uc, SPR_FONTS_CYRILLIC_SHA_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_shcha_uc, SPR_FONTS_CYRILLIC_SHCHA_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_hard_sign_uc, SPR_FONTS_CYRILLIC_HARD_SIGN_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_yeru_uc, SPR_FONTS_CYRILLIC_YERU_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_soft_sign_uc, SPR_FONTS_CYRILLIC_SOFT_SIGN_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_e_uc, SPR_FONTS_CYRILLIC_E_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_yu_uc, SPR_FONTS_CYRILLIC_YU_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ya_uc, SPR_FONTS_CYRILLIC_YA_UPPER - SPR_FONTS_BEGIN },

    { UnicodeChar::cyrillic_a, 'a' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_be, SPR_FONTS_CYRILLIC_BE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ve, SPR_FONTS_CYRILLIC_VE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ghe, SPR_FONTS_CYRILLIC_GHE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_de, SPR_FONTS_CYRILLIC_DE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ie, 'e' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_zhe, SPR_FONTS_CYRILLIC_ZHE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ze, SPR_FONTS_CYRILLIC_ZE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_i, SPR_FONTS_CYRILLIC_I_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_short_i, SPR_FONTS_CYRILLIC_SHORT_I_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ka, SPR_FONTS_CYRILLIC_KA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_el, SPR_FONTS_CYRILLIC_EL_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_em, SPR_FONTS_CYRILLIC_EM_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_en, SPR_FONTS_CYRILLIC_EN_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_o, 'o' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_pe, SPR_FONTS_CYRILLIC_PE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_er, 'p' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_es, 'c' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_te, SPR_FONTS_CYRILLIC_TE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_u, 'y' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_ef, SPR_FONTS_CYRILLIC_EF_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ha, 'x' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_tse, SPR_FONTS_CYRILLIC_TSE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_che, SPR_FONTS_CYRILLIC_CHE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_sha, SPR_FONTS_CYRILLIC_SHA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_shcha, SPR_FONTS_CYRILLIC_SHCHA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_hard_sign, SPR_FONTS_CYRILLIC_HARD_SIGN_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_yeru, SPR_FONTS_CYRILLIC_YERU_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_soft_sign, SPR_FONTS_CYRILLIC_SOFT_SIGN_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_e, SPR_FONTS_CYRILLIC_E_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_yu, SPR_FONTS_CYRILLIC_YU_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ya, SPR_FONTS_CYRILLIC_YA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_io, 235 - kCSSpriteFontOffset }, // Looks just like ë
    { UnicodeChar::cyrillic_ukrainian_ie, SPR_FONTS_CYRILLIC_UKRAINIAN_IE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_dze, 's' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_dotted_i, 'i' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_yi, 239 - kCSSpriteFontOffset }, // Looks just like ï
    { UnicodeChar::cyrillic_je, 'J' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillic_ghe_upturn_uc, SPR_FONTS_CYRILLIC_GHE_UPTURN_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillic_ghe_upturn, SPR_FONTS_CYRILLIC_GHE_UPTURN_LOWER - SPR_FONTS_BEGIN },

    // Punctuation
    { UnicodeChar::left_brace, SPR_FONTS_LEFT_BRACE - SPR_FONTS_BEGIN },
    { UnicodeChar::vertical_bar, SPR_FONTS_VERTICAL_BAR - SPR_FONTS_BEGIN },
    { UnicodeChar::right_brace, SPR_FONTS_RIGHT_BRACE - SPR_FONTS_BEGIN },
    { UnicodeChar::tilde, SPR_FONTS_TILDE - SPR_FONTS_BEGIN },
    { UnicodeChar::non_breaking_space, ' ' - kCSSpriteFontOffset },
    { UnicodeChar::interpunct, SPR_FONTS_INTERPUNCT - SPR_FONTS_BEGIN },
    { UnicodeChar::multiplication_sign, CSChar::cross - kCSSpriteFontOffset },
    { UnicodeChar::en_dash, '-' - kCSSpriteFontOffset },
    { UnicodeChar::em_dash, '-' - kCSSpriteFontOffset },
    { UnicodeChar::single_quote_open, '`' - kCSSpriteFontOffset },
    { UnicodeChar::single_quote_end, '\'' - kCSSpriteFontOffset },
    { UnicodeChar::single_german_quote_open, ',' - kCSSpriteFontOffset },
    { UnicodeChar::german_quote_open, SPR_FONTS_GERMAN_OPENQUOTES - SPR_FONTS_BEGIN },
    { UnicodeChar::bullet, CSChar::bullet - kCSSpriteFontOffset },
    { UnicodeChar::ellipsis, SPR_FONTS_ELLIPSIS - SPR_FONTS_BEGIN },
    { UnicodeChar::narrow_non_breaking_space, ' ' - kCSSpriteFontOffset },
    { UnicodeChar::quote_open, CSChar::quote_open - kCSSpriteFontOffset },
    { UnicodeChar::quote_close, CSChar::quote_close - kCSSpriteFontOffset },

    // Currency
    { UnicodeChar::guilder, SPR_FONTS_GUILDER_SIGN - SPR_FONTS_BEGIN },
    { UnicodeChar::euro, CSChar::euro - kCSSpriteFontOffset },
    { UnicodeChar::rouble, SPR_FONTS_ROUBLE_SIGN - SPR_FONTS_BEGIN },

    // Dingbats
    { UnicodeChar::up, CSChar::up - kCSSpriteFontOffset },
    { UnicodeChar::small_up, CSChar::small_up - kCSSpriteFontOffset },
    { UnicodeChar::right, CSChar::right - kCSSpriteFontOffset },
    { UnicodeChar::down, CSChar::down - kCSSpriteFontOffset },
    { UnicodeChar::small_down, CSChar::small_down - kCSSpriteFontOffset },
    { UnicodeChar::left, CSChar::left - kCSSpriteFontOffset },
    { UnicodeChar::air, CSChar::air - kCSSpriteFontOffset },
    { UnicodeChar::tick, CSChar::tick - kCSSpriteFontOffset },
    { UnicodeChar::plus, '+' - kCSSpriteFontOffset },
    { UnicodeChar::minus, '-' - kCSSpriteFontOffset },

    // Emoji
    { UnicodeChar::cross, CSChar::cross - kCSSpriteFontOffset },
    { UnicodeChar::water, CSChar::water - kCSSpriteFontOffset },
    { UnicodeChar::eye, SPR_FONTS_EYE - SPR_FONTS_BEGIN },
    { UnicodeChar::road, CSChar::road - kCSSpriteFontOffset },
    { UnicodeChar::railway, CSChar::railway - kCSSpriteFontOffset },

    // Misc
    { UnicodeChar::superscript_minus_one, CSChar::superscript_minus_one - kCSSpriteFontOffset },
};

static char32_t _smallestCodepointValue = 0;
static char32_t _biggestCodepointValue = 0;

/**
 *
 *  rct2: 0x006C19AC
 */
void FontSpriteInitialiseCharacters()
{
    // Compute min and max that helps avoiding lookups for no reason.
    _smallestCodepointValue = std::numeric_limits<char32_t>::max();
    for (const auto& entry : codepointOffsetMap)
    {
        _smallestCodepointValue = std::min(_smallestCodepointValue, entry.first);
        _biggestCodepointValue = std::max(_biggestCodepointValue, entry.first);
    }

    for (const auto& fontStyle : FontStyles)
    {
        int32_t glyphOffset = EnumValue(fontStyle) * SPR_FONTS_GLYPH_COUNT;
        for (auto glyphIndex = 0u; glyphIndex < SPR_FONTS_GLYPH_COUNT; glyphIndex++)
        {
            const G1Element* g1 = GfxGetG1Element(glyphIndex + SPR_FONTS_BEGIN + glyphOffset);
            int32_t width = 0;
            if (g1 != nullptr)
            {
                width = g1->width + (2 * g1->xOffset) - 1;
            }
            _spriteFontCharacterWidths[EnumValue(fontStyle)][glyphIndex] = static_cast<uint8_t>(width);
        }
    }

    Drawing::ScrollingText::initialiseBitmaps();
}

int32_t FontSpriteGetCodepointOffset(int32_t codepoint)
{
    // Only search the table when its in range of the map.
    if (static_cast<char32_t>(codepoint) >= _smallestCodepointValue
        && static_cast<char32_t>(codepoint) <= _biggestCodepointValue)
    {
        auto result = codepointOffsetMap.find(codepoint);
        if (result != codepointOffsetMap.end())
            return result->second;
    }

    if (codepoint < 32 || codepoint >= 256)
        codepoint = '?';

    return codepoint - 32;
}

int32_t FontSpriteGetCodepointWidth(FontStyle fontStyle, int32_t codepoint)
{
    int32_t glyphIndex = FontSpriteGetCodepointOffset(codepoint);
    auto baseFontIndex = EnumValue(fontStyle);

    if (glyphIndex >= static_cast<int32_t>(std::size(_spriteFontCharacterWidths[baseFontIndex])))
    {
        LOG_WARNING("Invalid glyph index %u", glyphIndex);
        glyphIndex = 0;
    }
    return _spriteFontCharacterWidths[baseFontIndex][glyphIndex];
}

ImageId FontSpriteGetCodepointSprite(FontStyle fontStyle, int32_t codepoint)
{
    auto codePointOffset = FontSpriteGetCodepointOffset(codepoint);
    int32_t offset = EnumValue(fontStyle) * SPR_FONTS_GLYPH_COUNT;

    return ImageId(SPR_FONTS_BEGIN + offset + codePointOffset, OpenRCT2::Drawing::Colour::black);
}

int32_t FontGetLineHeight(FontStyle fontStyle)
{
    auto fontSize = EnumValue(fontStyle);
#ifndef DISABLE_TTF
    if (LocalisationService_UseTrueTypeFont())
    {
        return gCurrentTTFFontSet->size[fontSize].line_height;
    }
#endif // DISABLE_TTF
    return kSpriteFontLineHeight[fontSize];
}

int32_t FontGetLineHeightSmall(FontStyle fontStyle)
{
    return FontGetLineHeight(fontStyle) / 2;
}

bool FontSupportsStringSprite(const utf8* text)
{
    const utf8* src = text;

    uint32_t codepoint;
    while ((codepoint = UTF8GetNext(src, &src)) != 0)
    {
        bool supported = false;

        if ((codepoint >= 32 && codepoint < 256)
            || (codepoint >= UnicodeChar::cyrillic_a_uc && codepoint <= UnicodeChar::cyrillic_ya))
        {
            supported = true;
        }

        auto result = codepointOffsetMap.find(codepoint);
        if (result != codepointOffsetMap.end())
            supported = true;

        if (!supported)
        {
            return false;
        }
    }
    return true;
}

bool FontSupportsStringTTF(const utf8* text, FontStyle fontStyle)
{
#ifndef DISABLE_TTF
    const utf8* src = text;
    const TTF_Font* font = gCurrentTTFFontSet->size[EnumValue(fontStyle)].font;
    if (font == nullptr)
    {
        return false;
    }

    uint32_t codepoint;
    while ((codepoint = UTF8GetNext(src, &src)) != 0)
    {
        bool supported = TTFProvidesGlyph(font, codepoint);
        if (!supported)
        {
            return false;
        }
    }
    return true;
#else
    return false;
#endif // DISABLE_TTF
}

bool FontSupportsString(const utf8* text, FontStyle fontStyle)
{
    if (LocalisationService_UseTrueTypeFont())
    {
        return FontSupportsStringTTF(text, fontStyle);
    }

    return FontSupportsStringSprite(text);
}
