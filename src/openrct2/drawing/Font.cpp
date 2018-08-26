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
        case FORMAT_ENDQUOTES:
            return 34 - 32;

        case FORMAT_UP:
            return 160 - 32;
        case FORMAT_INVERTEDEXCLAMATION:
            return 161 - 32;
        case FORMAT_POUND:
            return 163 - 32;

        case FORMAT_YEN:
            return 165 - 32;

        case FORMAT_COPYRIGHT:
            return 169 - 32;
        case FORMAT_DOWN:
            return 170 - 32;
        case FORMAT_LEFTGUILLEMET:
            return 171 - 32;
        case FORMAT_TICK:
            return 172 - 32;
        case FORMAT_CROSS:
            return 173 - 32;

        case FORMAT_RIGHT:
            return 175 - 32;
        case FORMAT_DEGREE:
            return 176 - 32;
        case FORMAT_SYMBOL_RAILWAY:
            return 177 - 32;
        case FORMAT_SQUARED:
            return 178 - 32;

        case FORMAT_OPENQUOTES:
            return 180 - 32;
        case FORMAT_EURO:
            return 181 - 32;
        case FORMAT_SYMBOL_ROAD:
            return 182 - 32;
        case FORMAT_SYMBOL_FLAG:
            return 183 - 32;
        case FORMAT_APPROX:
            return 184 - 32;
        case FORMAT_POWERNEGATIVEONE:
            return 185 - 32;
        case FORMAT_BULLET:
            return 186 - 32;
        case FORMAT_RIGHTGUILLEMET:
            return 187 - 32;
        case FORMAT_SMALLUP:
            return 188 - 32;
        case FORMAT_SMALLDOWN:
            return 189 - 32;
        case FORMAT_LEFT:
            return 190 - 32;
        case FORMAT_INVERTEDQUESTION:
            return 191 - 32;

        case UNICODE_A_OGONEK_UC:
            return RCT2_A_OGONEK_UC - 32;
        case UNICODE_C_ACUTE_UC:
            return RCT2_C_ACUTE_UC - 32;
        case UNICODE_E_OGONEK_UC:
            return RCT2_E_OGONEK_UC - 32;
        case UNICODE_N_ACUTE_UC:
            return RCT2_N_ACUTE_UC - 32;
        case UNICODE_L_STROKE_UC:
            return RCT2_L_STROKE_UC - 32;
        case UNICODE_S_ACUTE_UC:
            return RCT2_S_ACUTE_UC - 32;
        case UNICODE_Z_DOT_UC:
            return RCT2_Z_DOT_UC - 32;
        case UNICODE_Z_ACUTE_UC:
            return RCT2_Z_ACUTE_UC - 32;

        case UNICODE_A_OGONEK:
            return RCT2_A_OGONEK - 32;
        case UNICODE_C_ACUTE:
            return RCT2_C_ACUTE - 32;
        case UNICODE_E_OGONEK:
            return RCT2_E_OGONEK - 32;
        case UNICODE_N_ACUTE:
            return RCT2_N_ACUTE - 32;
        case UNICODE_L_STROKE:
            return RCT2_L_STROKE - 32;
        case UNICODE_S_ACUTE:
            return RCT2_S_ACUTE - 32;
        case UNICODE_Z_DOT:
            return RCT2_Z_DOT - 32;
        case UNICODE_Z_ACUTE:
            return RCT2_Z_ACUTE - 32;

        // Render capital sharp-S (ẞ) with lowercase sprite (ß)
        case UNICODE_CAPITAL_SHARP_S:
            return 223 - 32;

        // Norwegian/Danish
        case UNICODE_AE_UC:
            return SPR_G2_AE_UPPER - SPR_CHAR_START;
        case UNICODE_O_STROKE_UC:
            return SPR_G2_O_STROKE_UPPER - SPR_CHAR_START;
        case UNICODE_AE:
            return SPR_G2_AE_LOWER - SPR_CHAR_START;
        case UNICODE_O_STROKE:
            return SPR_G2_O_STROKE_LOWER - SPR_CHAR_START;

        case UNICODE_DINGBATS_PLUS:
            return 11;
        case UNICODE_DINGBATS_MINUS:
            return 13;

        // Cyrillic
        case UNICODE_CYRILLIC_A_UC:
            return 'A' - 32;
        case UNICODE_CYRILLIC_BE_UC:
            return SPR_G2_CYRILLIC_BE_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_VE_UC:
            return 'B' - 32;
        case UNICODE_CYRILLIC_GHE_UC:
            return SPR_G2_CYRILLIC_GHE_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_DE_UC:
            return SPR_G2_CYRILLIC_DE_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_IE_UC:
            return 'E' - 32;
        case UNICODE_CYRILLIC_ZHE_UC:
            return SPR_G2_CYRILLIC_ZHE_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_ZE_UC:
            return SPR_G2_CYRILLIC_ZE_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_I_UC:
            return SPR_G2_CYRILLIC_I_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_SHORT_I_UC:
            return SPR_G2_CYRILLIC_SHORT_I_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_KA_UC:
            return 'K' - 32;
        case UNICODE_CYRILLIC_EL_UC:
            return SPR_G2_CYRILLIC_EL_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_EM_UC:
            return 'M' - 32;
        case UNICODE_CYRILLIC_EN_UC:
            return 'H' - 32;
        case UNICODE_CYRILLIC_O_UC:
            return 'O' - 32;
        case UNICODE_CYRILLIC_PE_UC:
            return SPR_G2_CYRILLIC_PE_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_ER_UC:
            return 'P' - 32;
        case UNICODE_CYRILLIC_ES_UC:
            return 'C' - 32;
        case UNICODE_CYRILLIC_TE_UC:
            return 'T' - 32;
        case UNICODE_CYRILLIC_U_UC:
            return SPR_G2_CYRILLIC_U_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_EF_UC:
            return SPR_G2_CYRILLIC_EF_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_HA_UC:
            return 'X' - 32;
        case UNICODE_CYRILLIC_TSE_UC:
            return SPR_G2_CYRILLIC_TSE_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_CHE_UC:
            return SPR_G2_CYRILLIC_CHE_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_SHA_UC:
            return SPR_G2_CYRILLIC_SHA_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_SHCHA_UC:
            return SPR_G2_CYRILLIC_SHCHA_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_HARD_SIGN_UC:
            return SPR_G2_CYRILLIC_HARD_SIGN_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_YERU_UC:
            return SPR_G2_CYRILLIC_YERU_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_SOFT_SIGN_UC:
            return SPR_G2_CYRILLIC_SOFT_SIGN_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_E_UC:
            return SPR_G2_CYRILLIC_E_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_YU_UC:
            return SPR_G2_CYRILLIC_YU_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_YA_UC:
            return SPR_G2_CYRILLIC_YA_UPPER - SPR_CHAR_START;

        case UNICODE_CYRILLIC_A:
            return 'a' - 32;
        case UNICODE_CYRILLIC_BE:
            return SPR_G2_CYRILLIC_BE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_VE:
            return SPR_G2_CYRILLIC_VE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_GHE:
            return SPR_G2_CYRILLIC_GHE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_DE:
            return SPR_G2_CYRILLIC_DE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_IE:
            return 'e' - 32;
        case UNICODE_CYRILLIC_ZHE:
            return SPR_G2_CYRILLIC_ZHE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_ZE:
            return SPR_G2_CYRILLIC_ZE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_I:
            return SPR_G2_CYRILLIC_I_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_SHORT_I:
            return SPR_G2_CYRILLIC_SHORT_I_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_KA:
            return SPR_G2_CYRILLIC_KA_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_EL:
            return SPR_G2_CYRILLIC_EL_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_EM:
            return SPR_G2_CYRILLIC_EM_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_EN:
            return SPR_G2_CYRILLIC_EN_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_O:
            return 'o' - 32;
        case UNICODE_CYRILLIC_PE:
            return SPR_G2_CYRILLIC_PE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_ER:
            return 'p' - 32;
        case UNICODE_CYRILLIC_ES:
            return 'c' - 32;
        case UNICODE_CYRILLIC_TE:
            return SPR_G2_CYRILLIC_TE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_U:
            return 'y' - 32;
        case UNICODE_CYRILLIC_EF:
            return SPR_G2_CYRILLIC_EF_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_HA:
            return 'x' - 32;
        case UNICODE_CYRILLIC_TSE:
            return SPR_G2_CYRILLIC_TSE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_CHE:
            return SPR_G2_CYRILLIC_CHE_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_SHA:
            return SPR_G2_CYRILLIC_SHA_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_SHCHA:
            return SPR_G2_CYRILLIC_SHCHA_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_HARD_SIGN:
            // Not a typo, there is no glyph, use the upper case variant.
            return SPR_G2_CYRILLIC_HARD_SIGN_UPPER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_YERU:
            return SPR_G2_CYRILLIC_YERU_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_SOFT_SIGN:
            return SPR_G2_CYRILLIC_SOFT_SIGN_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_E:
            return SPR_G2_CYRILLIC_E_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_YU:
            return SPR_G2_CYRILLIC_YU_LOWER - SPR_CHAR_START;
        case UNICODE_CYRILLIC_YA:
            return SPR_G2_CYRILLIC_YA_LOWER - SPR_CHAR_START;

        // Looks just like Ë.
        case UNICODE_CYRILLIC_IO_UC:
            return 171;
        case UNICODE_CYRILLIC_IO:
            return 203;

        case FORMAT_GERMAN_OPENQUOTES:
            return SPR_G2_GERMAN_OPENQUOTES - SPR_CHAR_START;

        case FORMAT_SINGLE_OPENQUOTE:
            return 64;
        case FORMAT_SINGLE_ENDQUOTE:
            return 7;
        case FORMAT_GERMAN_SINGLE_OPENQUOTE:
            return 12;

        case UNICODE_GUILDER_SIGN:
            return SPR_G2_GUILDER_SIGN - SPR_CHAR_START;

        // Turkish
        case UNICODE_G_BREVE_UC:
            return SPR_G2_G_BREVE_UPPER - SPR_CHAR_START;
        case UNICODE_I_WITH_DOT_UC:
            return SPR_G2_I_WITH_DOT_UPPER - SPR_CHAR_START;
        case UNICODE_S_CEDILLA_UC:
            return SPR_G2_S_CEDILLA_UPPER - SPR_CHAR_START;
        case UNICODE_G_BREVE:
            return SPR_G2_G_BREVE_LOWER - SPR_CHAR_START;
        case UNICODE_I_WITHOUT_DOT:
            return SPR_G2_I_WITHOUT_DOT_LOWER - SPR_CHAR_START;
        case UNICODE_S_CEDILLA:
            return SPR_G2_S_CEDILLA_LOWER - SPR_CHAR_START;

        case UNICODE_INTERPUNCT:
            return SPR_G2_INTERPUNCT - SPR_CHAR_START;
        case UNICODE_ELLIPSIS:
            return SPR_G2_ELLIPSIS - SPR_CHAR_START;

        // Romanian
        case UNICODE_A_BREVE_UC:
            return SPR_G2_A_BREVE_UPPER - SPR_CHAR_START;
        case UNICODE_A_BREVE:
            // Render as â, there is no visual difference on this scale.
            return 194;
        case UNICODE_S_COMMA_UC:
            // Also no visual difference.
            return SPR_G2_S_CEDILLA_UPPER - SPR_CHAR_START;
        case UNICODE_S_COMMA:
            return SPR_G2_S_CEDILLA_LOWER - SPR_CHAR_START;
        case UNICODE_T_COMMA_UC:
            return SPR_G2_T_COMMA_UPPER - SPR_CHAR_START;
        case UNICODE_T_COMMA:
            return SPR_G2_T_COMMA_LOWER - SPR_CHAR_START;

        // This is to catch capitalised versions of the guilder sign
        case UNICODE_F_WITH_HOOK_UC:
            return 'F' - 32;

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
            case FORMAT_ENDQUOTES:
            case FORMAT_UP:
            case FORMAT_INVERTEDEXCLAMATION:
            case FORMAT_POUND:
            case FORMAT_YEN:
            case FORMAT_COPYRIGHT:
            case FORMAT_DOWN:
            case FORMAT_LEFTGUILLEMET:
            case FORMAT_TICK:
            case FORMAT_CROSS:
            case FORMAT_RIGHT:
            case FORMAT_DEGREE:
            case FORMAT_SYMBOL_RAILWAY:
            case FORMAT_SQUARED:
            case FORMAT_OPENQUOTES:
            case FORMAT_EURO:
            case FORMAT_SYMBOL_ROAD:
            case FORMAT_SYMBOL_FLAG:
            case FORMAT_APPROX:
            case FORMAT_POWERNEGATIVEONE:
            case FORMAT_BULLET:
            case FORMAT_RIGHTGUILLEMET:
            case FORMAT_SMALLUP:
            case FORMAT_SMALLDOWN:
            case FORMAT_LEFT:
            case FORMAT_INVERTEDQUESTION:

            case UNICODE_A_OGONEK_UC:
            case UNICODE_C_ACUTE_UC:
            case UNICODE_E_OGONEK_UC:
            case UNICODE_N_ACUTE_UC:
            case UNICODE_L_STROKE_UC:
            case UNICODE_S_ACUTE_UC:
            case UNICODE_Z_DOT_UC:
            case UNICODE_Z_ACUTE_UC:

            case UNICODE_A_OGONEK:
            case UNICODE_C_ACUTE:
            case UNICODE_E_OGONEK:
            case UNICODE_N_ACUTE:
            case UNICODE_L_STROKE:
            case UNICODE_S_ACUTE:
            case UNICODE_Z_DOT:
            case UNICODE_Z_ACUTE:

            case UNICODE_CYRILLIC_IO_UC:
            case UNICODE_CYRILLIC_IO:

            case FORMAT_GERMAN_OPENQUOTES:
            case FORMAT_SINGLE_OPENQUOTE:
            case FORMAT_SINGLE_ENDQUOTE:
            case FORMAT_GERMAN_SINGLE_OPENQUOTE:

            case UNICODE_GUILDER_SIGN:

            case UNICODE_G_BREVE_UC:
            case UNICODE_I_WITH_DOT_UC:
            case UNICODE_S_CEDILLA_UC:
            case UNICODE_G_BREVE:
            case UNICODE_I_WITHOUT_DOT:
            case UNICODE_S_CEDILLA:

            case UNICODE_INTERPUNCT:
            case UNICODE_ELLIPSIS:

            case UNICODE_A_BREVE_UC:
            case UNICODE_A_BREVE:
            case UNICODE_S_COMMA_UC:
            case UNICODE_S_COMMA:
            case UNICODE_T_COMMA_UC:
            case UNICODE_T_COMMA:

                supported = true;
                break;
            default:
                if ((codepoint >= 32 && codepoint < 256)
                    || (codepoint >= UNICODE_CYRILLIC_A_UC && codepoint <= UNICODE_CYRILLIC_YA))
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
