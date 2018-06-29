/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/Util.hpp"
#include "../localisation/FormatCodes.h"
#include "../localisation/Language.h"
#include "../localisation/LocalisationService.h"
#include "../sprites.h"
#include "Drawing.h"
#include "Font.h"
#include "TTF.h"

static constexpr const int32_t SpriteFontLineHeight[] = { 6, 10, 10, 18 };

static uint8_t _spriteFontCharacterWidths[896];

#ifndef NO_TTF
TTFFontSetDescriptor *gCurrentTTFFontSet;
#endif // NO_TTF

/**
 *
 *  rct2: 0x006C19AC
 */
void font_sprite_initialise_characters()
{
    uint8_t* pCharacterWidth = _spriteFontCharacterWidths;
    for (int32_t fontSize = 0; fontSize < FONT_SIZE_COUNT; fontSize++) {
        int32_t glyphOffset = fontSize * FONT_SPRITE_GLYPH_COUNT;
        for (uint8_t glyphIndex = 0; glyphIndex < FONT_SPRITE_GLYPH_COUNT; glyphIndex++) {
            const rct_g1_element * g1 = gfx_get_g1_element(glyphIndex + SPR_CHAR_START + glyphOffset);
            if (g1 != nullptr)
            {
                int32_t width = g1->width + 2 * g1->x_offset;
                width += fontSize == FONT_SIZE_BIG ? 1 : -1;
                if (glyphIndex >= (FORMAT_ARGUMENT_CODE_START - 32) && glyphIndex < (FORMAT_COLOUR_CODE_END - 32)) {
                    width = 0;
                }
                *pCharacterWidth++ = (uint8_t)width;
            }
        }
    }

    scrolling_text_initialise_bitmaps();
}

int32_t font_sprite_get_codepoint_offset(int32_t codepoint)
{
    switch (codepoint) {
    case FORMAT_ENDQUOTES: return 34 - 32;

    case FORMAT_UP: return 160 - 32;
    case FORMAT_INVERTEDEXCLAMATION: return 161 - 32;
    case FORMAT_POUND: return 163 - 32;

    case FORMAT_YEN: return 165 - 32;



    case FORMAT_COPYRIGHT: return 169 - 32;
    case FORMAT_DOWN: return 170 - 32;
    case FORMAT_LEFTGUILLEMET: return 171 - 32;
    case FORMAT_TICK: return 172 - 32;
    case FORMAT_CROSS: return 173 - 32;

    case FORMAT_RIGHT: return 175 - 32;
    case FORMAT_DEGREE: return 176 - 32;
    case FORMAT_SYMBOL_RAILWAY: return 177 - 32;
    case FORMAT_SQUARED: return 178 - 32;

    case FORMAT_OPENQUOTES: return 180 - 32;
    case FORMAT_EURO: return 181 - 32;
    case FORMAT_SYMBOL_ROAD: return 182 - 32;
    case FORMAT_SYMBOL_FLAG: return 183 - 32;
    case FORMAT_APPROX: return 184 - 32;
    case FORMAT_POWERNEGATIVEONE: return 185 - 32;
    case FORMAT_BULLET: return 186 - 32;
    case FORMAT_RIGHTGUILLEMET: return 187 - 32;
    case FORMAT_SMALLUP: return 188 - 32;
    case FORMAT_SMALLDOWN: return 189 - 32;
    case FORMAT_LEFT: return 190 - 32;
    case FORMAT_INVERTEDQUESTION: return 191 - 32;

    case UNICODE_A_OGONEK_UC: return RCT2_A_OGONEK_UC - 32;
    case UNICODE_C_ACUTE_UC: return RCT2_C_ACUTE_UC - 32;
    case UNICODE_E_OGONEK_UC: return RCT2_E_OGONEK_UC - 32;
    case UNICODE_N_ACUTE_UC: return RCT2_N_ACUTE_UC - 32;
    case UNICODE_L_STROKE_UC: return RCT2_L_STROKE_UC - 32;
    case UNICODE_S_ACUTE_UC: return RCT2_S_ACUTE_UC - 32;
    case UNICODE_Z_DOT_UC: return RCT2_Z_DOT_UC - 32;
    case UNICODE_Z_ACUTE_UC: return RCT2_Z_ACUTE_UC - 32;

    case UNICODE_A_OGONEK: return RCT2_A_OGONEK - 32;
    case UNICODE_C_ACUTE: return RCT2_C_ACUTE - 32;
    case UNICODE_E_OGONEK: return RCT2_E_OGONEK - 32;
    case UNICODE_N_ACUTE: return RCT2_N_ACUTE - 32;
    case UNICODE_L_STROKE: return RCT2_L_STROKE - 32;
    case UNICODE_S_ACUTE: return RCT2_S_ACUTE - 32;
    case UNICODE_Z_DOT: return RCT2_Z_DOT - 32;
    case UNICODE_Z_ACUTE: return RCT2_Z_ACUTE - 32;

    // Render capital sharp-S (ẞ) with lowercase sprite (ß)
    case UNICODE_CAPITAL_SHARP_S: return 223 - 32;

    case UNICODE_DINGBATS_PLUS: return 11;
    case UNICODE_DINGBATS_MINUS: return 13;

    default:
        if (codepoint < 32 || codepoint >= 256) codepoint = '?';
        return codepoint - 32;
    }
}

int32_t font_sprite_get_codepoint_width(uint16_t fontSpriteBase, int32_t codepoint)
{
    if (fontSpriteBase == (uint16_t)FONT_SPRITE_BASE_MEDIUM_DARK ||
        fontSpriteBase == (uint16_t)FONT_SPRITE_BASE_MEDIUM_EXTRA_DARK)
    {
        fontSpriteBase = (uint16_t)FONT_SPRITE_BASE_MEDIUM;
    }

    int32_t spriteFontIdx = fontSpriteBase + font_sprite_get_codepoint_offset(codepoint);
    if (spriteFontIdx < 0 || spriteFontIdx >= (int32_t)Util::CountOf(_spriteFontCharacterWidths))
    {
        log_warning("Invalid font index %u", spriteFontIdx);
        spriteFontIdx = 0;
    }
    return _spriteFontCharacterWidths[spriteFontIdx];
}

int32_t font_sprite_get_codepoint_sprite(int32_t fontSpriteBase, int32_t codepoint)
{
    return SPR_CHAR_START + (IMAGE_TYPE_REMAP | (fontSpriteBase + font_sprite_get_codepoint_offset(codepoint)));
}

int32_t font_get_size_from_sprite_base(uint16_t spriteBase)
{
    switch (spriteBase) {
    case FONT_SPRITE_BASE_TINY:
        return 0;
    case FONT_SPRITE_BASE_SMALL:
        return 1;
    default:
    case FONT_SPRITE_BASE_MEDIUM:
        return 2;
    case FONT_SPRITE_BASE_BIG:
        return 3;
    }
}

int32_t font_get_line_height(int32_t fontSpriteBase)
{
    int32_t fontSize = font_get_size_from_sprite_base(fontSpriteBase);
#ifndef NO_TTF
    if (LocalisationService_UseTrueTypeFont()) {
        return gCurrentTTFFontSet->size[fontSize].line_height;
    } else {
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

bool font_supports_string_sprite(const utf8 *text)
{
    const utf8 *src = text;

    uint32_t codepoint;
    while ((codepoint = utf8_get_next(src, &src)) != 0) {
        bool supported = false;
        switch (codepoint) {
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

            supported = true;
            break;
        default:
            if (codepoint >= 32 && codepoint < 256) {
                supported = true;
            }
            break;
        }
        if (!supported) {
            return false;
        }
    }
    return true;
}

bool font_supports_string_ttf(const utf8 *text, int32_t fontSize)
{
#ifndef NO_TTF
    const utf8 *src = text;
    const TTF_Font *font = gCurrentTTFFontSet->size[fontSize].font;
    if (font == nullptr) {
        return false;
    }

    uint32_t codepoint;
    while ((codepoint = utf8_get_next(src, &src)) != 0) {
        bool supported = ttf_provides_glyph(font, codepoint);
        if (!supported) {
            return false;
        }
    }
    return true;
#else
    return false;
#endif // NO_TTF
}

bool font_supports_string(const utf8 *text, int32_t fontSize)
{
    if (LocalisationService_UseTrueTypeFont()) {
        return font_supports_string_ttf(text, fontSize);
    } else {
        return font_supports_string_sprite(text);
    }
}
