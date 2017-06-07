#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../rct2/addresses.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "drawing.h"
#include "font.h"

static const sint32 SpriteFontLineHeight[] = { 6, 10, 10, 18 };

#ifdef NO_RCT2
static uint8 _spriteFontCharacterWidths[896];
#else
static uint8 *_spriteFontCharacterWidths = RCT2_ADDRESS(RCT2_ADDRESS_FONT_CHAR_WIDTH, uint8);
#endif

#ifndef NO_TTF
TTFFontSetDescriptor *gCurrentTTFFontSet;
#endif // NO_TTF

/**
 *
 *  rct2: 0x006C19AC
 */
void font_sprite_initialise_characters()
{
    uint8* pCharacterWidth = _spriteFontCharacterWidths;
    for (sint32 fontSize = 0; fontSize < FONT_SIZE_COUNT; fontSize++) {
        sint32 glyphOffset = fontSize * FONT_SPRITE_GLYPH_COUNT;
        for (uint8 glyphIndex = 0; glyphIndex < FONT_SPRITE_GLYPH_COUNT; glyphIndex++) {
            rct_g1_element g1 = g1Elements[glyphIndex + SPR_CHAR_START + glyphOffset];

            sint32 width = fontSize == FONT_SIZE_BIG ? g1.width + 1 : g1.width - 1;
            if (glyphIndex >= (FORMAT_ARGUMENT_CODE_START - 32) && glyphIndex < (FORMAT_COLOUR_CODE_END - 32)) {
                width = 0;
            }
            *pCharacterWidth++ = (uint8)width;
        }
    }

    scrolling_text_initialise_bitmaps();
}

sint32 font_sprite_get_codepoint_offset(sint32 codepoint)
{
    switch (codepoint) {
    case FORMAT_ENDQUOTES: return 34 - 32;

    case FORMAT_AMINUSCULE: return 159 - 32;
    case FORMAT_UP: return 160 - 32;
    case FORMAT_SYMBOL_i: return 160 - 32;
    case FORMAT_CENT: return 162 - 32;
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
    default:
        if (codepoint < 32 || codepoint >= 256) codepoint = '?';
        return codepoint - 32;
    }
}

sint32 font_sprite_get_codepoint_width(sint32 fontSpriteBase, sint32 codepoint)
{
    return _spriteFontCharacterWidths[fontSpriteBase + font_sprite_get_codepoint_offset(codepoint)];
}

sint32 font_sprite_get_codepoint_sprite(sint32 fontSpriteBase, sint32 codepoint)
{
    return SPR_CHAR_START + (IMAGE_TYPE_REMAP | (fontSpriteBase + font_sprite_get_codepoint_offset(codepoint)));
}

sint32 font_get_size_from_sprite_base(uint16 spriteBase)
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

sint32 font_get_line_height(sint32 fontSpriteBase)
{
    sint32 fontSize = font_get_size_from_sprite_base(fontSpriteBase);
#ifndef NO_TTF
    if (gUseTrueTypeFont) {
        return gCurrentTTFFontSet->size[fontSize].line_height;
    } else {
#endif // NO_TTF
        return SpriteFontLineHeight[fontSize];
#ifndef NO_TTF
    }
#endif // NO_TTF
}

sint32 font_get_line_height_small(sint32 fontSpriteBase)
{
    return font_get_line_height(fontSpriteBase) / 2;
}

bool font_supports_string_sprite(const utf8 *text)
{
    const utf8 *src = text;

    uint32 codepoint;
    while ((codepoint = utf8_get_next(src, &src)) != 0) {
        bool supported = false;
        switch (codepoint) {
        case FORMAT_ENDQUOTES:
        case FORMAT_AMINUSCULE:
        case FORMAT_UP:
        case FORMAT_SYMBOL_i:
        case FORMAT_CENT:
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

bool font_supports_string_ttf(const utf8 *text, sint32 fontSize)
{
#ifndef NO_TTF
    const utf8 *src = text;
    const TTF_Font *font = gCurrentTTFFontSet->size[fontSize].font;
    if (font == NULL) {
        return false;
    }

    uint32 codepoint;
    while ((codepoint = utf8_get_next(src, &src)) != 0) {
        bool supported = TTF_GlyphIsProvided(font, (uint16)codepoint);
        if (!supported) {
            return false;
        }
    }
    return true;
#else
    return false;
#endif // NO_TTF
}

bool font_supports_string(const utf8 *text, sint32 fontSize)
{
    if (gUseTrueTypeFont) {
        return font_supports_string_ttf(text, fontSize);
    } else {
        return font_supports_string_sprite(text);
    }
}
