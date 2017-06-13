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

#ifndef _DRAWING_FONT_H_
#define _DRAWING_FONT_H_

#include "../common.h"

enum {
    FONT_SIZE_TINY = 2,
    FONT_SIZE_SMALL = 0,
    FONT_SIZE_MEDIUM = 1,
    FONT_SIZE_BIG = 3,
    FONT_SIZE_COUNT = 4
};

enum {
    FONT_SPRITE_GLYPH_COUNT = 224,
    FONT_SPRITE_BASE_MEDIUM_EXTRA_DARK = -2,
    FONT_SPRITE_BASE_MEDIUM_DARK = -1,

    FONT_SPRITE_BASE_TINY = 448,
    FONT_SPRITE_BASE_SMALL = 0,
    FONT_SPRITE_BASE_MEDIUM = 224,
    FONT_SPRITE_BASE_BIG = 672
};

#ifndef NO_TTF
typedef struct FT_FaceRec_*  FT_Face;
typedef struct TTFFont {
    FT_Face face;
} TTFFont;

typedef struct TTFFontDescriptor {
    const utf8 *filename;
    const utf8 *font_name;
    sint32 ptSize;
    sint32 offset_x;
    sint32 offset_y;
    sint32 line_height;
    TTFFont * font;
} TTFFontDescriptor;

typedef struct  TTFFontSetDescriptor {
    TTFFontDescriptor size[FONT_SIZE_COUNT];
} TTFFontSetDescriptor;

extern TTFFontSetDescriptor *gCurrentTTFFontSet;
#endif // NO_TTF

void font_sprite_initialise_characters();
sint32 font_sprite_get_codepoint_offset(sint32 codepoint);
sint32 font_sprite_get_codepoint_width(sint32 fontSpriteBase, sint32 codepoint);
sint32 font_sprite_get_codepoint_sprite(sint32 fontSpriteBase, sint32 codepoint);
sint32 font_get_size_from_sprite_base(uint16 spriteBase);
sint32 font_get_line_height(sint32 fontSpriteBase);
sint32 font_get_line_height_small(sint32 fontSpriteBase);
bool font_supports_string_sprite(const utf8 *text);
bool font_supports_string_ttf(const utf8 *text, sint32 fontSize);
bool font_supports_string(const utf8 *text, sint32 fontSize);

#endif
