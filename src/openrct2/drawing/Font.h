/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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

struct _TTF_Font;
using TTF_Font = _TTF_Font;
struct TTFFontDescriptor {
    const utf8 *filename;
    const utf8 *font_name;
    sint32 ptSize;
    sint32 offset_x;
    sint32 offset_y;
    sint32 line_height;
    sint32 hinting_threshold;
    TTF_Font * font;
};

struct TTFFontSetDescriptor {
    TTFFontDescriptor size[FONT_SIZE_COUNT];
};

extern TTFFontSetDescriptor *gCurrentTTFFontSet;

#endif // NO_TTF

void font_sprite_initialise_characters();
sint32 font_sprite_get_codepoint_offset(sint32 codepoint);
sint32 font_sprite_get_codepoint_width(uint16 fontSpriteBase, sint32 codepoint);
sint32 font_sprite_get_codepoint_sprite(sint32 fontSpriteBase, sint32 codepoint);
sint32 font_get_size_from_sprite_base(uint16 spriteBase);
sint32 font_get_line_height(sint32 fontSpriteBase);
sint32 font_get_line_height_small(sint32 fontSpriteBase);
bool font_supports_string_sprite(const utf8 *text);
bool font_supports_string_ttf(const utf8 *text, sint32 fontSize);
bool font_supports_string(const utf8 *text, sint32 fontSize);

#endif
