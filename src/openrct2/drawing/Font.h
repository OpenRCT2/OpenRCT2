/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
namespace OpenRCT2
{
    constexpr const uint16_t FONT_SPRITE_GLYPH_COUNT = 224;

    enum class FontStyles : uint8_t
    {
        Small = 0,
        Medium = 1,
        Tiny = 2,
    };

    constexpr const uint8_t FontStyleCount = 3;
    constexpr const std::array<FontStyles, FontStyleCount> FontStyles = { FontStyles::Small, FontStyles::Medium, FontStyles::Tiny };

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
    int32_t FontSpriteGetCodepointWidth(FontStyles fontStyle, int32_t codepoint);
    ImageId FontSpriteGetCodepointSprite(FontStyles fontStyle, int32_t codepoint);
    int32_t FontGetLineHeight(FontStyles fontStyle);
    int32_t FontGetLineHeightSmall(FontStyles fontStyle);
    bool FontSupportsStringSprite(const utf8* text);
    bool FontSupportsStringTTF(const utf8* text, FontStyles fontStyle);
    bool FontSupportsString(const utf8* text, FontStyles fontStyle);
} // namespace OpenRCT2
