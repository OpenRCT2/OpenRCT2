/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Font.h"

#include "../core/Util.hpp"
#include "../localisation/FormatCodes.h"
#include "../localisation/Language.h"
#include "../localisation/LocalisationService.h"
#include "../sprites.h"
#include "Drawing.h"
#include "TTF.h"

static constexpr const int32_t SpriteFontLineHeight[FONT_SIZE_COUNT] = { 6, 10, 10 };

static uint8_t _spriteFontCharacterWidths[FONT_SIZE_COUNT][FONT_SPRITE_GLYPH_COUNT];
static uint8_t _additionalSpriteFontCharacterWidth[FONT_SIZE_COUNT][SPR_G2_GLYPH_COUNT] = {};

#ifndef NO_TTF
TTFFontSetDescriptor* gCurrentTTFFontSet;
#endif // NO_TTF

/**
 *
 *  rct2: 0x006C19AC
 */
void font_sprite_initialise_characters()
{
    for (int32_t fontSize = 0; fontSize < FONT_SIZE_COUNT; fontSize++)
    {
        int32_t glyphOffset = fontSize * FONT_SPRITE_GLYPH_COUNT;
        for (uint8_t glyphIndex = 0; glyphIndex < FONT_SPRITE_GLYPH_COUNT; glyphIndex++)
        {
            const rct_g1_element* g1 = gfx_get_g1_element(glyphIndex + SPR_CHAR_START + glyphOffset);
            int32_t width = 0;
            if (g1 != nullptr)
            {
                if (glyphIndex < (FORMAT_ARGUMENT_CODE_START - 32) || glyphIndex >= (FORMAT_COLOUR_CODE_END - 32))
                {
                    width = (g1->width + 2 * g1->x_offset) - 1;
                }
            }

            _spriteFontCharacterWidths[fontSize][glyphIndex] = (uint8_t)width;
        }
    }

    for (uint8_t fontSize : { FONT_SIZE_SMALL, FONT_SIZE_MEDIUM, FONT_SIZE_TINY })
    {
        int32_t glyphOffset = fontSize * SPR_G2_GLYPH_COUNT;
        for (int32_t glyphIndex = 0; glyphIndex < SPR_G2_GLYPH_COUNT; glyphIndex++)
        {
            const rct_g1_element* g1 = gfx_get_g1_element(glyphIndex + SPR_G2_CHAR_BEGIN + glyphOffset);
            int32_t width = 0;
            if (g1 != nullptr)
            {
                width = g1->width + (2 * g1->x_offset) - 1;
            }

            _additionalSpriteFontCharacterWidth[fontSize][glyphIndex] = (uint8_t)width;
        }
    }

    scrolling_text_initialise_bitmaps();
}

int32_t font_sprite_get_codepoint_offset(int32_t codepoint)
{
    switch (codepoint)
    {
        case UnicodeChar::quote_close:
            return 34 - 32;

        case UnicodeChar::up:
            return 160 - 32;

        case UnicodeChar::down:
            return 170 - 32;
        case UnicodeChar::tick:
            return 172 - 32;
        case UnicodeChar::cross:
            return 173 - 32;

        case UnicodeChar::right:
            return 175 - 32;
        case UnicodeChar::railway:
            return 177 - 32;

        case UnicodeChar::quote_open:
            return 180 - 32;
        case UnicodeChar::euro:
            return 181 - 32;
        case UnicodeChar::road:
            return 182 - 32;
        case UnicodeChar::air:
            return 183 - 32;
        case UnicodeChar::water:
            return 184 - 32;
        case UnicodeChar::superscript_minus_one:
            return 185 - 32;
        case UnicodeChar::bullet:
            return 186 - 32;
        case UnicodeChar::small_up:
            return 188 - 32;
        case UnicodeChar::small_down:
            return 189 - 32;
        case UnicodeChar::left:
            return 190 - 32;

        case UnicodeChar::a_ogonek_uc:
            return RCT2_A_OGONEK_UC - 32;
        case UnicodeChar::c_acute_uc:
            return RCT2_C_ACUTE_UC - 32;
        case UnicodeChar::e_ogonek_uc:
            return RCT2_E_OGONEK_UC - 32;
        case UnicodeChar::n_acute_uc:
            return RCT2_N_ACUTE_UC - 32;
        case UnicodeChar::l_stroke_uc:
            return RCT2_L_STROKE_UC - 32;
        case UnicodeChar::s_acute_uc:
            return RCT2_S_ACUTE_UC - 32;
        case UnicodeChar::z_dot_uc:
            return RCT2_Z_DOT_UC - 32;
        case UnicodeChar::z_acute_uc:
            return RCT2_Z_ACUTE_UC - 32;

        case UnicodeChar::a_ogonek:
            return RCT2_A_OGONEK - 32;
        case UnicodeChar::c_acute:
            return RCT2_C_ACUTE - 32;
        case UnicodeChar::e_ogonek:
            return RCT2_E_OGONEK - 32;
        case UnicodeChar::n_acute:
            return RCT2_N_ACUTE - 32;
        case UnicodeChar::l_stroke:
            return RCT2_L_STROKE - 32;
        case UnicodeChar::s_acute:
            return RCT2_S_ACUTE - 32;
        case UnicodeChar::z_dot:
            return RCT2_Z_DOT - 32;
        case UnicodeChar::z_acute:
            return RCT2_Z_ACUTE - 32;

        // Render capital sharp-S (ẞ) with lowercase sprite (ß)
        case UnicodeChar::sharp_s_uc:
            return 223 - 32;

        // Norwegian/Danish
        case UnicodeChar::ae_uc:
            return SPR_G2_AE_UPPER - SPR_CHAR_START;
        case UnicodeChar::o_stroke_uc:
            return SPR_G2_O_STROKE_UPPER - SPR_CHAR_START;
        case UnicodeChar::ae:
            return SPR_G2_AE_LOWER - SPR_CHAR_START;
        case UnicodeChar::o_stroke:
            return SPR_G2_O_STROKE_LOWER - SPR_CHAR_START;

        case UnicodeChar::plus:
            return 11;
        case UnicodeChar::minus:
            return 13;

        // Cyrillic
        case UnicodeChar::cyrillic_a_uc:
            return 'A' - 32;
        case UnicodeChar::cyrillic_be_uc:
            return SPR_G2_CYRILLIC_BE_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ve_uc:
            return 'B' - 32;
        case UnicodeChar::cyrillic_ghe_uc:
            return SPR_G2_CYRILLIC_GHE_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_de_uc:
            return SPR_G2_CYRILLIC_DE_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ie_uc:
            return 'E' - 32;
        case UnicodeChar::cyrillic_zhe_uc:
            return SPR_G2_CYRILLIC_ZHE_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ze_uc:
            return SPR_G2_CYRILLIC_ZE_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_i_uc:
            return SPR_G2_CYRILLIC_I_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_short_i_uc:
            return SPR_G2_CYRILLIC_SHORT_I_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ka_uc:
            return 'K' - 32;
        case UnicodeChar::cyrillic_el_uc:
            return SPR_G2_CYRILLIC_EL_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_em_uc:
            return 'M' - 32;
        case UnicodeChar::cyrillic_en_uc:
            return 'H' - 32;
        case UnicodeChar::cyrillic_o_uc:
            return 'O' - 32;
        case UnicodeChar::cyrillic_pe_uc:
            return SPR_G2_CYRILLIC_PE_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_er_uc:
            return 'P' - 32;
        case UnicodeChar::cyrillic_es_uc:
            return 'C' - 32;
        case UnicodeChar::cyrillic_te_uc:
            return 'T' - 32;
        case UnicodeChar::cyrillic_u_uc:
            return SPR_G2_CYRILLIC_U_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ef_uc:
            return SPR_G2_CYRILLIC_EF_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ha_uc:
            return 'X' - 32;
        case UnicodeChar::cyrillic_tse_uc:
            return SPR_G2_CYRILLIC_TSE_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_che_uc:
            return SPR_G2_CYRILLIC_CHE_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_sha_uc:
            return SPR_G2_CYRILLIC_SHA_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_shcha_uc:
            return SPR_G2_CYRILLIC_SHCHA_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_hard_sign_uc:
            return SPR_G2_CYRILLIC_HARD_SIGN_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_yeru_uc:
            return SPR_G2_CYRILLIC_YERU_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_soft_sign_uc:
            return SPR_G2_CYRILLIC_SOFT_SIGN_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_e_uc:
            return SPR_G2_CYRILLIC_E_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_yu_uc:
            return SPR_G2_CYRILLIC_YU_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ya_uc:
            return SPR_G2_CYRILLIC_YA_UPPER - SPR_CHAR_START;

        case UnicodeChar::cyrillic_a:
            return 'a' - 32;
        case UnicodeChar::cyrillic_be:
            return SPR_G2_CYRILLIC_BE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ve:
            return SPR_G2_CYRILLIC_VE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ghe:
            return SPR_G2_CYRILLIC_GHE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_de:
            return SPR_G2_CYRILLIC_DE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ie:
            return 'e' - 32;
        case UnicodeChar::cyrillic_zhe:
            return SPR_G2_CYRILLIC_ZHE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ze:
            return SPR_G2_CYRILLIC_ZE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_i:
            return SPR_G2_CYRILLIC_I_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_short_i:
            return SPR_G2_CYRILLIC_SHORT_I_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ka:
            return SPR_G2_CYRILLIC_KA_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_el:
            return SPR_G2_CYRILLIC_EL_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_em:
            return SPR_G2_CYRILLIC_EM_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_en:
            return SPR_G2_CYRILLIC_EN_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_o:
            return 'o' - 32;
        case UnicodeChar::cyrillic_pe:
            return SPR_G2_CYRILLIC_PE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_er:
            return 'p' - 32;
        case UnicodeChar::cyrillic_es:
            return 'c' - 32;
        case UnicodeChar::cyrillic_te:
            return SPR_G2_CYRILLIC_TE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_u:
            return 'y' - 32;
        case UnicodeChar::cyrillic_ef:
            return SPR_G2_CYRILLIC_EF_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ha:
            return 'x' - 32;
        case UnicodeChar::cyrillic_tse:
            return SPR_G2_CYRILLIC_TSE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_che:
            return SPR_G2_CYRILLIC_CHE_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_sha:
            return SPR_G2_CYRILLIC_SHA_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_shcha:
            return SPR_G2_CYRILLIC_SHCHA_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_hard_sign:
            // Not a typo, there is no glyph, use the upper case variant.
            return SPR_G2_CYRILLIC_HARD_SIGN_UPPER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_yeru:
            return SPR_G2_CYRILLIC_YERU_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_soft_sign:
            return SPR_G2_CYRILLIC_SOFT_SIGN_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_e:
            return SPR_G2_CYRILLIC_E_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_yu:
            return SPR_G2_CYRILLIC_YU_LOWER - SPR_CHAR_START;
        case UnicodeChar::cyrillic_ya:
            return SPR_G2_CYRILLIC_YA_LOWER - SPR_CHAR_START;

        // Looks just like Ë.
        case UnicodeChar::cyrillic_io_uc:
            return 171;
        case UnicodeChar::cyrillic_io:
            return 203;

        case UnicodeChar::german_quote_open:
            return SPR_G2_GERMAN_OPENQUOTES - SPR_CHAR_START;

        case UnicodeChar::single_quote_open:
            return 64;
        case UnicodeChar::single_quote_end:
            return 7;
        case UnicodeChar::single_german_quote_open:
            return 12;

        case UnicodeChar::guilder:
            return SPR_G2_GUILDER_SIGN - SPR_CHAR_START;

        // Turkish
        case UnicodeChar::g_breve_uc:
            return SPR_G2_G_BREVE_UPPER - SPR_CHAR_START;
        case UnicodeChar::i_with_dot_uc:
            return SPR_G2_I_WITH_DOT_UPPER - SPR_CHAR_START;
        case UnicodeChar::s_cedilla_uc:
            return SPR_G2_S_CEDILLA_UPPER - SPR_CHAR_START;
        case UnicodeChar::g_breve:
            return SPR_G2_G_BREVE_LOWER - SPR_CHAR_START;
        case UnicodeChar::i_without_dot:
            return SPR_G2_I_WITHOUT_DOT_LOWER - SPR_CHAR_START;
        case UnicodeChar::s_cedilla:
            return SPR_G2_S_CEDILLA_LOWER - SPR_CHAR_START;

        case UnicodeChar::interpunct:
            return SPR_G2_INTERPUNCT - SPR_CHAR_START;
        case UnicodeChar::ellipsis:
            return SPR_G2_ELLIPSIS - SPR_CHAR_START;

        // Romanian
        case UnicodeChar::a_breve_uc:
            return SPR_G2_A_BREVE_UPPER - SPR_CHAR_START;
        case UnicodeChar::a_breve:
            // Render as â, there is no visual difference on this scale.
            return 194;
        case UnicodeChar::s_comma_uc:
            // Also no visual difference.
            return SPR_G2_S_CEDILLA_UPPER - SPR_CHAR_START;
        case UnicodeChar::s_comma:
            return SPR_G2_S_CEDILLA_LOWER - SPR_CHAR_START;
        case UnicodeChar::t_comma_uc:
            return SPR_G2_T_COMMA_UPPER - SPR_CHAR_START;
        case UnicodeChar::t_comma:
            return SPR_G2_T_COMMA_LOWER - SPR_CHAR_START;

        // This is to catch capitalised versions of the guilder sign
        case UnicodeChar::f_with_hook_uc:
            return 'F' - 32;

        // Czech
        case UnicodeChar::c_caron_uc:
            return SPR_G2_C_CARON_UPPER - SPR_CHAR_START;
        case UnicodeChar::c_caron:
            return SPR_G2_C_CARON_LOWER - SPR_CHAR_START;
        case UnicodeChar::y_acute_uc:
            return SPR_G2_Y_ACUTE_UPPER - SPR_CHAR_START;
        case UnicodeChar::y_acute:
            return SPR_G2_Y_ACUTE_LOWER - SPR_CHAR_START;

        // Hungarian
        case UnicodeChar::o_double_acute_uc:
            return SPR_G2_O_DOUBLE_ACUTE_UPPER - SPR_CHAR_START;
        case UnicodeChar::o_double_acute:
            return SPR_G2_O_DOUBLE_ACUTE_LOWER - SPR_CHAR_START;
        case UnicodeChar::u_double_acute_uc:
            return SPR_G2_U_DOUBLE_ACUTE_UPPER - SPR_CHAR_START;
        case UnicodeChar::u_double_acute:
            return SPR_G2_U_DOUBLE_ACUTE_LOWER - SPR_CHAR_START;

        default:
            if (codepoint < 32 || codepoint >= 256)
                codepoint = '?';
            return codepoint - 32;
    }
}

int32_t font_sprite_get_codepoint_width(uint16_t fontSpriteBase, int32_t codepoint)
{
    if (fontSpriteBase == (uint16_t)FONT_SPRITE_BASE_MEDIUM_DARK
        || fontSpriteBase == (uint16_t)FONT_SPRITE_BASE_MEDIUM_EXTRA_DARK)
    {
        fontSpriteBase = (uint16_t)FONT_SPRITE_BASE_MEDIUM;
    }

    int32_t glyphIndex = font_sprite_get_codepoint_offset(codepoint);
    int32_t baseFontIndex = font_get_font_index_from_sprite_base(fontSpriteBase);
    if (glyphIndex >= FONT_SPRITE_GLYPH_COUNT)
    {
        glyphIndex = (SPR_CHAR_START + glyphIndex) - SPR_G2_CHAR_BEGIN;

        if (glyphIndex >= (int32_t)Util::CountOf(_additionalSpriteFontCharacterWidth[baseFontIndex]))
        {
            log_warning("Invalid glyph index %u", glyphIndex);
            glyphIndex = 0;
        }
        return _additionalSpriteFontCharacterWidth[baseFontIndex][glyphIndex];
    }
    else if (glyphIndex < 0 || glyphIndex >= (int32_t)FONT_SPRITE_GLYPH_COUNT)
    {
        log_warning("Invalid glyph index %u", glyphIndex);
        glyphIndex = 0;
    }
    return _spriteFontCharacterWidths[baseFontIndex][glyphIndex];
}

int32_t font_sprite_get_codepoint_sprite(int32_t fontSpriteBase, int32_t codepoint)
{
    auto codePointOffset = font_sprite_get_codepoint_offset(codepoint);
    if (codePointOffset > FONT_SPRITE_GLYPH_COUNT)
    {
        fontSpriteBase = font_get_font_index_from_sprite_base(fontSpriteBase) * SPR_G2_GLYPH_COUNT;
    }

    return SPR_CHAR_START + (IMAGE_TYPE_REMAP | (fontSpriteBase + codePointOffset));
}

int32_t font_get_font_index_from_sprite_base(uint16_t spriteBase)
{
    switch (spriteBase)
    {
        case FONT_SPRITE_BASE_TINY:
            return FONT_SIZE_TINY;
        case FONT_SPRITE_BASE_SMALL:
            return FONT_SIZE_SMALL;
        default:
        case FONT_SPRITE_BASE_MEDIUM:
            return FONT_SIZE_MEDIUM;
    }
}

int32_t font_get_size_from_sprite_base(uint16_t spriteBase)
{
    switch (spriteBase)
    {
        case FONT_SPRITE_BASE_TINY:
            return 0;
        case FONT_SPRITE_BASE_SMALL:
            return 1;
        default:
        case FONT_SPRITE_BASE_MEDIUM:
            return 2;
    }
}

int32_t font_get_line_height(int32_t fontSpriteBase)
{
    int32_t fontSize = font_get_size_from_sprite_base(fontSpriteBase);
#ifndef NO_TTF
    if (LocalisationService_UseTrueTypeFont())
    {
        return gCurrentTTFFontSet->size[fontSize].line_height;
    }
    else
    {
#endif // NO_TTF
        return SpriteFontLineHeight[fontSize];
#ifndef NO_TTF
    }
#endif // NO_TTF
}

int32_t font_get_line_height_small(int32_t fontSpriteBase)
{
    return font_get_line_height(fontSpriteBase) / 2;
}

bool font_supports_string_sprite(const utf8* text)
{
    const utf8* src = text;

    uint32_t codepoint;
    while ((codepoint = utf8_get_next(src, &src)) != 0)
    {
        bool supported = false;
        switch (codepoint)
        {
            // Latin alphabet
            case UnicodeChar::ae_uc:
            case UnicodeChar::o_stroke_uc:
            case UnicodeChar::y_acute_uc:
            case UnicodeChar::ae:
            case UnicodeChar::o_stroke:
            case UnicodeChar::y_acute:
            case UnicodeChar::a_breve_uc:
            case UnicodeChar::a_breve:
            case UnicodeChar::a_ogonek_uc:
            case UnicodeChar::a_ogonek:
            case UnicodeChar::c_acute_uc:
            case UnicodeChar::c_acute:
            case UnicodeChar::c_caron_uc:
            case UnicodeChar::c_caron:
            case UnicodeChar::e_ogonek_uc:
            case UnicodeChar::e_ogonek:
            case UnicodeChar::g_breve_uc:
            case UnicodeChar::g_breve:
            case UnicodeChar::i_with_dot_uc:
            case UnicodeChar::i_without_dot:
            case UnicodeChar::l_stroke_uc:
            case UnicodeChar::l_stroke:
            case UnicodeChar::n_acute_uc:
            case UnicodeChar::n_acute:
            case UnicodeChar::o_double_acute_uc:
            case UnicodeChar::o_double_acute:
            case UnicodeChar::s_acute_uc:
            case UnicodeChar::s_acute:
            case UnicodeChar::s_cedilla_uc:
            case UnicodeChar::s_cedilla:
            case UnicodeChar::u_double_acute_uc:
            case UnicodeChar::u_double_acute:
            case UnicodeChar::z_acute_uc:
            case UnicodeChar::z_acute:
            case UnicodeChar::z_dot_uc:
            case UnicodeChar::z_dot:
            case UnicodeChar::f_with_hook_uc:
            case UnicodeChar::s_comma_uc:
            case UnicodeChar::s_comma:
            case UnicodeChar::t_comma_uc:
            case UnicodeChar::t_comma:
            case UnicodeChar::sharp_s_uc:

                // Cyrillic alphabet
            case UnicodeChar::cyrillic_io_uc:
            case UnicodeChar::cyrillic_io:

                // Punctuation
            case UnicodeChar::leftguillemet:
            case UnicodeChar::rightguillemet:
            case UnicodeChar::interpunct:
            case UnicodeChar::single_quote_open:
            case UnicodeChar::single_quote_end:
            case UnicodeChar::single_german_quote_open:
            case UnicodeChar::german_quote_open:
            case UnicodeChar::bullet:
            case UnicodeChar::ellipsis:
            case UnicodeChar::quote_open:
            case UnicodeChar::quote_close:

                // Currency
            case UnicodeChar::guilder:
            case UnicodeChar::euro:

                // Dingbats
            case UnicodeChar::up:
            case UnicodeChar::small_up:
            case UnicodeChar::right:
            case UnicodeChar::down:
            case UnicodeChar::small_down:
            case UnicodeChar::left:
            case UnicodeChar::air:
            case UnicodeChar::tick:
            case UnicodeChar::plus:
            case UnicodeChar::minus:

                // Emoji
            case UnicodeChar::cross:
            case UnicodeChar::variation_selector:
            case UnicodeChar::water:
            case UnicodeChar::road:
            case UnicodeChar::railway:

                // Misc
            case UnicodeChar::superscript_minus_one:

                supported = true;
                break;
            default:
                if ((codepoint >= 32 && codepoint < 256)
                    || (codepoint >= UnicodeChar::cyrillic_a_uc && codepoint <= UnicodeChar::cyrillic_ya))
                {
                    supported = true;
                }
                break;
        }

        if (!supported)
        {
            return false;
        }
    }
    return true;
}

bool font_supports_string_ttf(const utf8* text, int32_t fontSize)
{
#ifndef NO_TTF
    const utf8* src = text;
    const TTF_Font* font = gCurrentTTFFontSet->size[fontSize].font;
    if (font == nullptr)
    {
        return false;
    }

    uint32_t codepoint;
    while ((codepoint = utf8_get_next(src, &src)) != 0)
    {
        bool supported = ttf_provides_glyph(font, codepoint);
        if (!supported)
        {
            return false;
        }
    }
    return true;
#else
    return false;
#endif // NO_TTF
}

bool font_supports_string(const utf8* text, int32_t fontSize)
{
    if (LocalisationService_UseTrueTypeFont())
    {
        return font_supports_string_ttf(text, fontSize);
    }
    else
    {
        return font_supports_string_sprite(text);
    }
}
