/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"
#include "../drawing/ImageId.hpp"

#include <array>

constexpr uint16_t kSpriteFontGlyphCount = 224;

enum class FontStyle : uint8_t
{
    Small = 0,
    Medium = 1,
    Tiny = 2,
};

constexpr uint8_t FontStyleCount = 3;
constexpr std::array<FontStyle, FontStyleCount> FontStyles = { FontStyle::Small, FontStyle::Medium, FontStyle::Tiny };

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

void FontSpriteInitialiseCharacters();
int32_t FontSpriteGetCodepointOffset(int32_t codepoint);
int32_t FontSpriteGetCodepointWidth(FontStyle fontStyle, int32_t codepoint);
ImageId FontSpriteGetCodepointSprite(FontStyle fontStyle, int32_t codepoint);
int32_t FontGetLineHeight(FontStyle fontStyle);
int32_t FontGetLineHeightSmall(FontStyle fontStyle);
bool FontSupportsStringSprite(const utf8* text);
bool FontSupportsStringTTF(const utf8* text, FontStyle fontStyle);
bool FontSupportsString(const utf8* text, FontStyle fontStyle);
