/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"
#include "../drawing/ImageId.hpp"

#include <array>

constexpr const uint16_t FONT_SPRITE_GLYPH_COUNT = 224;

enum class FontStyle : uint8_t
{
    Small = 0,
    Medium = 1,
    Tiny = 2,
};

constexpr const uint8_t FontStyleCount = 3;
constexpr const std::array<FontStyle, FontStyleCount> FontStyles = { FontStyle::Small, FontStyle::Medium, FontStyle::Tiny };

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
    TTFFontDescriptor size[FontStyleCount];
};

extern TTFFontSetDescriptor* gCurrentTTFFontSet;

#endif // NO_TTF

void font_sprite_initialise_characters();
int32_t font_sprite_get_codepoint_offset(int32_t codepoint);
int32_t font_sprite_get_codepoint_width(FontStyle fontStyle, int32_t codepoint);
ImageId font_sprite_get_codepoint_sprite(FontStyle fontStyle, int32_t codepoint);
int32_t font_get_line_height(FontStyle fontStyle);
int32_t font_get_line_height_small(FontStyle fontStyle);
bool font_supports_string_sprite(const utf8* text);
bool font_supports_string_ttf(const utf8* text, FontStyle fontStyle);
bool font_supports_string(const utf8* text, FontStyle fontStyle);
