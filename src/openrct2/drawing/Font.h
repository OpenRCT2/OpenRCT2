/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"

constexpr const uint16_t FONT_SPRITE_GLYPH_COUNT = 224;

enum
{
    FONT_SIZE_TINY = 2,
    FONT_SIZE_SMALL = 0,
    FONT_SIZE_MEDIUM = 1,
    FONT_SIZE_COUNT = 3
};

enum class FontSpriteBase : int16_t
{
    MEDIUM_EXTRA_DARK = -2,
    MEDIUM_DARK = -1,

    TINY = FONT_SIZE_TINY * FONT_SPRITE_GLYPH_COUNT,
    SMALL = FONT_SIZE_SMALL * FONT_SPRITE_GLYPH_COUNT,
    MEDIUM = FONT_SIZE_MEDIUM * FONT_SPRITE_GLYPH_COUNT,
};

#ifndef NO_TTF

struct InternalTTFFont;
using TTF_Font = InternalTTFFont;
struct TTFFontDescriptor
{
    const utf8* filename;
    const utf8* font_name;
    int32_t ptSize;
    int32_t offset_x;
    int32_t offset_y;
    int32_t line_height;
    int32_t hinting_threshold;
    TTF_Font* font;
};

struct TTFFontSetDescriptor
{
    TTFFontDescriptor size[FONT_SIZE_COUNT];
};

extern TTFFontSetDescriptor* gCurrentTTFFontSet;

#endif // NO_TTF

void font_sprite_initialise_characters();
int32_t font_sprite_get_codepoint_offset(int32_t codepoint);
int32_t font_sprite_get_codepoint_width(FontSpriteBase fontSpriteBase, int32_t codepoint);
int32_t font_sprite_get_codepoint_sprite(FontSpriteBase fontSpriteBase, int32_t codepoint);
int32_t font_get_font_index_from_sprite_base(FontSpriteBase spriteBase);
int32_t font_get_size_from_sprite_base(FontSpriteBase spriteBase);
int32_t font_get_line_height(FontSpriteBase fontSpriteBase);
int32_t font_get_line_height_small(FontSpriteBase fontSpriteBase);
bool font_supports_string_sprite(const utf8* text);
bool font_supports_string_ttf(const utf8* text, int32_t fontSize);
bool font_supports_string(const utf8* text, int32_t fontSize);
