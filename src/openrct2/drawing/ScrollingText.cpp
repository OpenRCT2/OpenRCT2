/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScrollingText.h"

#include "../config/Config.h"
#include "../core/CodepointView.hpp"
#include "../core/EnumUtils.hpp"
#include "../core/String.hpp"
#include "../interface/Colour.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../localisation/LocalisationService.h"
#include "../paint/Paint.h"
#include "../sprites.h"
#include "Drawing.h"
#include "TTF.h"

#include <cassert>
#include <mutex>

using namespace OpenRCT2;

struct DrawScrollText
{
    StringId string_id;
    uint8_t string_args[32];
    colour_t colour;
    uint16_t position;
    uint16_t mode;
    uint32_t id;
    uint8_t bitmap[64 * 40];
};

static DrawScrollText _drawScrollTextList[OpenRCT2::kMaxScrollingTextEntries];
static uint8_t _characterBitmaps[kSpriteFontGlyphCount + SPR_G2_GLYPH_COUNT][8];
static uint32_t _drawSCrollNextIndex = 0;
static std::mutex _scrollingTextMutex;

static void ScrollingTextSetBitmapForSprite(
    std::string_view text, int32_t scroll, uint8_t* bitmap, const int16_t* scrollPositionOffsets, colour_t colour);
static void ScrollingTextSetBitmapForTTF(
    std::string_view text, int32_t scroll, uint8_t* bitmap, const int16_t* scrollPositionOffsets, colour_t colour);

static void ScrollingTextInitialiseCharacterBitmaps(uint32_t glyphStart, uint16_t offset, uint16_t count, bool isAntiAliased)
{
    uint8_t drawingSurface[64];
    DrawPixelInfo dpi;
    dpi.bits = reinterpret_cast<uint8_t*>(&drawingSurface);
    dpi.width = 8;
    dpi.height = 8;

    for (int32_t i = 0; i < count; i++)
    {
        std::fill_n(drawingSurface, sizeof(drawingSurface), 0x00);
        GfxDrawSpriteSoftware(dpi, ImageId(glyphStart + (EnumValue(FontStyle::Tiny) * count) + i), { -1, 0 });

        for (int32_t x = 0; x < 8; x++)
        {
            uint8_t val = 0;
            for (int32_t y = 0; y < 8; y++)
            {
                val >>= 1;
                uint8_t pixel = dpi.bits[x + y * 8];
                if (pixel == 1 || (isAntiAliased && pixel == 2))
                {
                    val |= 0x80;
                }
            }
            _characterBitmaps[offset + i][x] = val;
        }
    }
};

static void ScrollingTextInitialiseScrollingText()
{
    for (int32_t i = 0; i < OpenRCT2::kMaxScrollingTextEntries; i++)
    {
        const int32_t imageId = SPR_SCROLLING_TEXT_START + i;

        // Initialize the scrolling text sprite.
        G1Element g1{};
        g1.offset = _drawScrollTextList[i].bitmap;
        g1.x_offset = -32;
        g1.y_offset = 0;
        g1.flags = G1_FLAG_HAS_TRANSPARENCY;
        g1.width = 64;
        g1.height = 40;
        g1.offset[0] = 0xFF;
        g1.offset[1] = 0xFF;
        g1.offset[14] = 0;
        g1.offset[15] = 0;
        g1.offset[16] = 0;
        g1.offset[17] = 0;

        GfxSetG1Element(imageId, &g1);
    }
}

void ScrollingTextInitialiseBitmaps()
{
    ScrollingTextInitialiseCharacterBitmaps(SPR_CHAR_START, 0, kSpriteFontGlyphCount, gTinyFontAntiAliased);
    ScrollingTextInitialiseCharacterBitmaps(SPR_G2_CHAR_BEGIN, kSpriteFontGlyphCount, SPR_G2_GLYPH_COUNT, false);
    ScrollingTextInitialiseScrollingText();
}

static uint8_t* FontSpriteGetCodepointBitmap(int32_t codepoint)
{
    auto offset = FontSpriteGetCodepointOffset(codepoint);
    if (offset >= kSpriteFontGlyphCount)
    {
        return _characterBitmaps[offset - (SPR_G2_CHAR_BEGIN - SPR_CHAR_START) + kSpriteFontGlyphCount];
    }

    return _characterBitmaps[offset];
}

static int32_t ScrollingTextGetMatchingOrOldest(
    StringId stringId, Formatter& ft, uint16_t scroll, uint16_t scrollingMode, colour_t colour)
{
    uint32_t oldestId = 0xFFFFFFFF;
    int32_t scrollIndex = -1;
    for (size_t i = 0; i < std::size(_drawScrollTextList); i++)
    {
        DrawScrollText* scrollText = &_drawScrollTextList[i];
        if (oldestId >= scrollText->id)
        {
            oldestId = scrollText->id;
            scrollIndex = static_cast<int32_t>(i);
        }

        // If exact match return the matching index
        if (scrollText->string_id == stringId
            && std::memcmp(scrollText->string_args, ft.Buf(), sizeof(scrollText->string_args)) == 0
            && scrollText->colour == colour && scrollText->position == scroll && scrollText->mode == scrollingMode)
        {
            scrollText->id = _drawSCrollNextIndex;
            return static_cast<int32_t>(i + SPR_SCROLLING_TEXT_START);
        }
    }
    return scrollIndex;
}

static void ScrollingTextFormat(utf8* dst, size_t size, DrawScrollText* scrollText)
{
    if (Config::Get().general.UpperCaseBanners)
    {
        FormatStringToUpper(dst, size, scrollText->string_id, scrollText->string_args);
    }
    else
    {
        FormatStringLegacy(dst, size, scrollText->string_id, scrollText->string_args);
    }
}

extern bool TempForScrollText;

consteval int16_t ScrollPos(const int16_t x, const int16_t y)
{
    return y * 64 + x;
}

// clang-format off
static constexpr int16_t kScrollPos0[] = {
    ScrollPos( 35, 12 ),
    ScrollPos( 36, 12 ),
    ScrollPos( 37, 11 ),
    ScrollPos( 38, 11 ),
    ScrollPos( 39, 10 ),
    ScrollPos( 40, 10 ),
    ScrollPos( 41,  9 ),
    ScrollPos( 42,  9 ),
    ScrollPos( 43,  8 ),
    ScrollPos( 44,  8 ),
    ScrollPos( 45,  7 ),
    ScrollPos( 46,  7 ),
    ScrollPos( 47,  6 ),
    ScrollPos( 48,  6 ),
    ScrollPos( 49,  5 ),
    ScrollPos( 50,  5 ),
    ScrollPos( 51,  4 ),
    ScrollPos( 52,  4 ),
    ScrollPos( 53,  3 ),
    ScrollPos( 54,  3 ),
    ScrollPos( 55,  2 ),
    ScrollPos( 56,  2 ),
    ScrollPos( 57,  1 ),
    ScrollPos( 58,  1 ),
    -1,
};
static constexpr int16_t kScrollPos1[] = {
    ScrollPos(  5,  1 ),
    ScrollPos(  6,  1 ),
    ScrollPos(  7,  2 ),
    ScrollPos(  8,  2 ),
    ScrollPos(  9,  3 ),
    ScrollPos( 10,  3 ),
    ScrollPos( 11,  4 ),
    ScrollPos( 12,  4 ),
    ScrollPos( 13,  5 ),
    ScrollPos( 14,  5 ),
    ScrollPos( 15,  6 ),
    ScrollPos( 16,  6 ),
    ScrollPos( 17,  7 ),
    ScrollPos( 18,  7 ),
    ScrollPos( 19,  8 ),
    ScrollPos( 20,  8 ),
    ScrollPos( 21,  9 ),
    ScrollPos( 22,  9 ),
    ScrollPos( 23, 10 ),
    ScrollPos( 24, 10 ),
    ScrollPos( 25, 11 ),
    ScrollPos( 26, 11 ),
    ScrollPos( 27, 12 ),
    ScrollPos( 28, 12 ),
    -1,
};
static constexpr int16_t kScrollPos2[] = {
    ScrollPos( 12,  1 ),
    ScrollPos( 13,  1 ),
    ScrollPos( 14,  2 ),
    ScrollPos( 15,  2 ),
    ScrollPos( 16,  3 ),
    ScrollPos( 17,  3 ),
    ScrollPos( 18,  4 ),
    ScrollPos( 19,  4 ),
    ScrollPos( 20,  5 ),
    ScrollPos( 21,  5 ),
    ScrollPos( 22,  6 ),
    ScrollPos( 23,  6 ),
    ScrollPos( 24,  7 ),
    ScrollPos( 25,  7 ),
    ScrollPos( 26,  8 ),
    ScrollPos( 27,  8 ),
    ScrollPos( 28,  9 ),
    ScrollPos( 29,  9 ),
    ScrollPos( 30, 10 ),
    ScrollPos( 31, 10 ),
    ScrollPos( 32, 10 ),
    ScrollPos( 33,  9 ),
    ScrollPos( 34,  9 ),
    ScrollPos( 35,  8 ),
    ScrollPos( 36,  8 ),
    ScrollPos( 37,  7 ),
    ScrollPos( 38,  7 ),
    ScrollPos( 39,  6 ),
    ScrollPos( 40,  6 ),
    ScrollPos( 41,  5 ),
    ScrollPos( 42,  5 ),
    ScrollPos( 43,  4 ),
    ScrollPos( 44,  4 ),
    ScrollPos( 45,  3 ),
    ScrollPos( 46,  3 ),
    ScrollPos( 47,  2 ),
    ScrollPos( 48,  2 ),
    ScrollPos( 49,  1 ),
    ScrollPos( 50,  1 ),
    -1,
};
static constexpr int16_t kScrollPos3[] = {
    ScrollPos( 16,  0 ),
    ScrollPos( 17,  1 ),
    ScrollPos( 18,  1 ),
    ScrollPos( 19,  2 ),
    ScrollPos( 20,  3 ),
    ScrollPos( 21,  3 ),
    ScrollPos( 22,  4 ),
    ScrollPos( 23,  5 ),
    ScrollPos( 24,  5 ),
    ScrollPos( 25,  6 ),
    ScrollPos( 26,  6 ),
    ScrollPos( 27,  7 ),
    ScrollPos( 28,  7 ),
    ScrollPos( 29,  8 ),
    ScrollPos( 30,  8 ),
    ScrollPos( 31,  9 ),
    ScrollPos( 32,  9 ),
    ScrollPos( 33, 10 ),
    ScrollPos( 34, 10 ),
    ScrollPos( 35, 11 ),
    ScrollPos( 36, 11 ),
    ScrollPos( 37, 12 ),
    ScrollPos( 38, 12 ),
    ScrollPos( 39, 13 ),
    ScrollPos( 40, 13 ),
    ScrollPos( 41, 14 ),
    ScrollPos( 42, 14 ),
    ScrollPos( 43, 14 ),
    ScrollPos( 44, 15 ),
    ScrollPos( 45, 15 ),
    ScrollPos( 46, 15 ),
    ScrollPos( 47, 16 ),
    ScrollPos( 48, 16 ),
    -1,
};
static constexpr int16_t kScrollPos4[] = {
    ScrollPos( 15, 17 ),
    ScrollPos( 16, 17 ),
    ScrollPos( 17, 16 ),
    ScrollPos( 18, 16 ),
    ScrollPos( 19, 16 ),
    ScrollPos( 20, 15 ),
    ScrollPos( 21, 15 ),
    ScrollPos( 22, 15 ),
    ScrollPos( 23, 14 ),
    ScrollPos( 24, 14 ),
    ScrollPos( 25, 13 ),
    ScrollPos( 26, 13 ),
    ScrollPos( 27, 12 ),
    ScrollPos( 28, 12 ),
    ScrollPos( 29, 11 ),
    ScrollPos( 30, 11 ),
    ScrollPos( 31, 10 ),
    ScrollPos( 32, 10 ),
    ScrollPos( 33,  9 ),
    ScrollPos( 34,  9 ),
    ScrollPos( 35,  8 ),
    ScrollPos( 36,  8 ),
    ScrollPos( 37,  7 ),
    ScrollPos( 38,  7 ),
    ScrollPos( 39,  6 ),
    ScrollPos( 40,  6 ),
    ScrollPos( 41,  5 ),
    ScrollPos( 42,  4 ),
    ScrollPos( 43,  4 ),
    ScrollPos( 44,  3 ),
    ScrollPos( 45,  2 ),
    ScrollPos( 46,  2 ),
    ScrollPos( 47,  1 ),
    ScrollPos( 48,  0 ),
    -1,
};
static constexpr int16_t kScrollPos5[] = {
    ScrollPos(  4, 12 ),
    ScrollPos(  5, 12 ),
    ScrollPos(  6, 11 ),
    ScrollPos(  7, 11 ),
    ScrollPos(  8, 10 ),
    ScrollPos(  9, 10 ),
    ScrollPos( 10,  9 ),
    ScrollPos( 11,  9 ),
    ScrollPos( 12,  8 ),
    ScrollPos( 13,  8 ),
    ScrollPos( 14,  7 ),
    ScrollPos( 15,  7 ),
    ScrollPos( 16,  6 ),
    ScrollPos( 17,  6 ),
    ScrollPos( 18,  5 ),
    ScrollPos( 19,  5 ),
    ScrollPos( 20,  4 ),
    ScrollPos( 21,  4 ),
    ScrollPos( 22,  3 ),
    ScrollPos( 23,  3 ),
    ScrollPos( 24,  2 ),
    ScrollPos( 25,  2 ),
    ScrollPos( 26,  1 ),
    ScrollPos( 27,  1 ),
    -1,
};
static constexpr int16_t kScrollPos6[] = {
    ScrollPos( 36,  1 ),
    ScrollPos( 37,  1 ),
    ScrollPos( 38,  2 ),
    ScrollPos( 39,  2 ),
    ScrollPos( 40,  3 ),
    ScrollPos( 41,  3 ),
    ScrollPos( 42,  4 ),
    ScrollPos( 43,  4 ),
    ScrollPos( 44,  5 ),
    ScrollPos( 45,  5 ),
    ScrollPos( 46,  6 ),
    ScrollPos( 47,  6 ),
    ScrollPos( 48,  7 ),
    ScrollPos( 49,  7 ),
    ScrollPos( 50,  8 ),
    ScrollPos( 51,  8 ),
    ScrollPos( 52,  9 ),
    ScrollPos( 53,  9 ),
    ScrollPos( 54, 10 ),
    ScrollPos( 55, 10 ),
    ScrollPos( 56, 11 ),
    ScrollPos( 57, 11 ),
    ScrollPos( 58, 12 ),
    ScrollPos( 59, 12 ),
    -1,
};
static constexpr int16_t kScrollPos7[] = {
    ScrollPos(  8, 11 ),
    ScrollPos(  9, 11 ),
    ScrollPos( 10, 10 ),
    ScrollPos( 11, 10 ),
    ScrollPos( 12,  9 ),
    ScrollPos( 13,  9 ),
    ScrollPos( 14,  8 ),
    ScrollPos( 15,  8 ),
    ScrollPos( 16,  7 ),
    ScrollPos( 17,  7 ),
    ScrollPos( 18,  6 ),
    ScrollPos( 19,  6 ),
    ScrollPos( 20,  5 ),
    ScrollPos( 21,  5 ),
    ScrollPos( 22,  4 ),
    ScrollPos( 23,  4 ),
    ScrollPos( 24,  3 ),
    ScrollPos( 25,  3 ),
    ScrollPos( 26,  2 ),
    ScrollPos( 27,  2 ),
    ScrollPos( 28,  1 ),
    ScrollPos( 29,  1 ),
    -1,
};
static constexpr int16_t kScrollPos8[] = {
    ScrollPos( 36,  2 ),
    ScrollPos( 37,  2 ),
    ScrollPos( 38,  3 ),
    ScrollPos( 39,  3 ),
    ScrollPos( 40,  4 ),
    ScrollPos( 41,  4 ),
    ScrollPos( 42,  5 ),
    ScrollPos( 43,  5 ),
    ScrollPos( 44,  6 ),
    ScrollPos( 45,  6 ),
    ScrollPos( 46,  7 ),
    ScrollPos( 47,  7 ),
    ScrollPos( 48,  8 ),
    ScrollPos( 49,  8 ),
    ScrollPos( 50,  9 ),
    ScrollPos( 51,  9 ),
    ScrollPos( 52, 10 ),
    ScrollPos( 53, 10 ),
    ScrollPos( 54, 11 ),
    ScrollPos( 55, 11 ),
    -1,
};
static constexpr int16_t kScrollPos9[] = {
    ScrollPos( 11,  9 ),
    ScrollPos( 12,  9 ),
    ScrollPos( 13,  9 ),
    ScrollPos( 14,  9 ),
    ScrollPos( 15,  9 ),
    ScrollPos( 16,  8 ),
    ScrollPos( 17,  8 ),
    ScrollPos( 18,  7 ),
    ScrollPos( 19,  7 ),
    ScrollPos( 20,  7 ),
    ScrollPos( 21,  6 ),
    ScrollPos( 22,  6 ),
    ScrollPos( 23,  5 ),
    ScrollPos( 24,  4 ),
    ScrollPos( 25,  4 ),
    ScrollPos( 26,  3 ),
    ScrollPos( 27,  3 ),
    ScrollPos( 28,  2 ),
    ScrollPos( 29,  1 ),
    -1,
};
static constexpr int16_t kScrollPos10[] = {
    ScrollPos( 34,  1 ),
    ScrollPos( 35,  2 ),
    ScrollPos( 36,  3 ),
    ScrollPos( 37,  3 ),
    ScrollPos( 38,  4 ),
    ScrollPos( 39,  4 ),
    ScrollPos( 40,  5 ),
    ScrollPos( 41,  6 ),
    ScrollPos( 42,  6 ),
    ScrollPos( 43,  7 ),
    ScrollPos( 44,  7 ),
    ScrollPos( 45,  7 ),
    ScrollPos( 46,  8 ),
    ScrollPos( 47,  8 ),
    ScrollPos( 48,  9 ),
    ScrollPos( 49,  9 ),
    ScrollPos( 50,  9 ),
    ScrollPos( 51,  9 ),
    ScrollPos( 52,  9 ),
    -1,
};
static constexpr int16_t kScrollPos11[] = {
    ScrollPos( 14, 10 ),
    ScrollPos( 15,  9 ),
    ScrollPos( 16,  9 ),
    ScrollPos( 17,  8 ),
    ScrollPos( 18,  8 ),
    ScrollPos( 19,  7 ),
    ScrollPos( 20,  7 ),
    ScrollPos( 21,  6 ),
    ScrollPos( 22,  6 ),
    ScrollPos( 23,  5 ),
    ScrollPos( 24,  5 ),
    ScrollPos( 25,  4 ),
    ScrollPos( 26,  4 ),
    ScrollPos( 27,  3 ),
    ScrollPos( 28,  3 ),
    ScrollPos( 29,  2 ),
    ScrollPos( 30,  2 ),
    ScrollPos( 31,  1 ),
    ScrollPos( 32,  1 ),
    ScrollPos( 33,  0 ),
    ScrollPos( 34,  0 ),
    -1,
};
static constexpr int16_t kScrollPos12[] = {
    ScrollPos( 33,  1 ),
    ScrollPos( 34,  2 ),
    ScrollPos( 35,  2 ),
    ScrollPos( 36,  3 ),
    ScrollPos( 37,  3 ),
    ScrollPos( 38,  4 ),
    ScrollPos( 39,  4 ),
    ScrollPos( 40,  5 ),
    ScrollPos( 41,  5 ),
    ScrollPos( 42,  6 ),
    ScrollPos( 43,  6 ),
    ScrollPos( 44,  7 ),
    ScrollPos( 45,  7 ),
    ScrollPos( 46,  8 ),
    ScrollPos( 47,  8 ),
    ScrollPos( 48,  9 ),
    ScrollPos( 49,  9 ),
    ScrollPos( 50, 10 ),
    ScrollPos( 51, 10 ),
    ScrollPos( 52, 11 ),
    ScrollPos( 53, 11 ),
    -1,
};
static constexpr int16_t kScrollPos13[] = {
    ScrollPos( 12, 11 ),
    ScrollPos( 13, 10 ),
    ScrollPos( 14, 10 ),
    ScrollPos( 15,  9 ),
    ScrollPos( 16,  9 ),
    ScrollPos( 17,  8 ),
    ScrollPos( 18,  8 ),
    ScrollPos( 19,  7 ),
    ScrollPos( 20,  7 ),
    ScrollPos( 21,  6 ),
    ScrollPos( 22,  6 ),
    ScrollPos( 23,  5 ),
    ScrollPos( 24,  5 ),
    ScrollPos( 25,  4 ),
    ScrollPos( 26,  4 ),
    ScrollPos( 27,  3 ),
    ScrollPos( 28,  3 ),
    ScrollPos( 29,  2 ),
    ScrollPos( 30,  2 ),
    ScrollPos( 31,  1 ),
    -1,
};
static constexpr int16_t kScrollPos14[] = {
    ScrollPos( 33,  1 ),
    ScrollPos( 34,  2 ),
    ScrollPos( 35,  2 ),
    ScrollPos( 36,  3 ),
    ScrollPos( 37,  3 ),
    ScrollPos( 38,  4 ),
    ScrollPos( 39,  4 ),
    ScrollPos( 40,  5 ),
    ScrollPos( 41,  5 ),
    ScrollPos( 42,  6 ),
    ScrollPos( 43,  6 ),
    ScrollPos( 44,  7 ),
    ScrollPos( 45,  7 ),
    ScrollPos( 46,  8 ),
    ScrollPos( 47,  8 ),
    ScrollPos( 48,  9 ),
    ScrollPos( 49,  9 ),
    ScrollPos( 50, 10 ),
    ScrollPos( 51, 10 ),
    ScrollPos( 52, 11 ),
    ScrollPos( 53, 11 ),
    -1,
};
static constexpr int16_t kScrollPos15[] = {
    ScrollPos( 10, 10 ),
    ScrollPos( 11, 10 ),
    ScrollPos( 12,  9 ),
    ScrollPos( 13,  9 ),
    ScrollPos( 14,  8 ),
    ScrollPos( 15,  8 ),
    ScrollPos( 16,  7 ),
    ScrollPos( 17,  7 ),
    ScrollPos( 18,  6 ),
    ScrollPos( 19,  6 ),
    ScrollPos( 20,  5 ),
    ScrollPos( 21,  5 ),
    ScrollPos( 22,  4 ),
    ScrollPos( 23,  4 ),
    ScrollPos( 24,  3 ),
    ScrollPos( 25,  3 ),
    ScrollPos( 26,  2 ),
    ScrollPos( 27,  2 ),
    ScrollPos( 28,  1 ),
    ScrollPos( 29,  1 ),
    ScrollPos( 30,  0 ),
    ScrollPos( 31,  0 ),
    -1,
};
static constexpr int16_t kScrollPos16[] = {
    ScrollPos( 33,  0 ),
    ScrollPos( 34,  0 ),
    ScrollPos( 35,  1 ),
    ScrollPos( 36,  1 ),
    ScrollPos( 37,  2 ),
    ScrollPos( 38,  2 ),
    ScrollPos( 39,  3 ),
    ScrollPos( 40,  3 ),
    ScrollPos( 41,  4 ),
    ScrollPos( 42,  4 ),
    ScrollPos( 43,  5 ),
    ScrollPos( 44,  5 ),
    ScrollPos( 45,  6 ),
    ScrollPos( 46,  6 ),
    ScrollPos( 47,  7 ),
    ScrollPos( 48,  7 ),
    ScrollPos( 49,  8 ),
    ScrollPos( 50,  8 ),
    ScrollPos( 51,  9 ),
    ScrollPos( 52,  9 ),
    ScrollPos( 53, 10 ),
    ScrollPos( 54, 10 ),
    -1,
};
static constexpr int16_t kScrollPos17[] = {
    ScrollPos(  6, 11 ),
    ScrollPos(  7, 11 ),
    ScrollPos(  8, 10 ),
    ScrollPos(  9, 10 ),
    ScrollPos( 10,  9 ),
    ScrollPos( 11,  9 ),
    ScrollPos( 12,  8 ),
    ScrollPos( 13,  8 ),
    ScrollPos( 14,  7 ),
    ScrollPos( 15,  7 ),
    ScrollPos( 16,  6 ),
    ScrollPos( 17,  6 ),
    ScrollPos( 18,  5 ),
    ScrollPos( 19,  5 ),
    ScrollPos( 20,  4 ),
    ScrollPos( 21,  4 ),
    ScrollPos( 22,  3 ),
    ScrollPos( 23,  3 ),
    ScrollPos( 24,  2 ),
    ScrollPos( 25,  2 ),
    ScrollPos( 26,  1 ),
    ScrollPos( 27,  1 ),
    ScrollPos( 28,  0 ),
    ScrollPos( 29,  0 ),
    -1,
};
static constexpr int16_t kScrollPos18[] = {
    ScrollPos( 34,  0 ),
    ScrollPos( 35,  0 ),
    ScrollPos( 36,  1 ),
    ScrollPos( 37,  1 ),
    ScrollPos( 38,  2 ),
    ScrollPos( 39,  2 ),
    ScrollPos( 40,  3 ),
    ScrollPos( 41,  3 ),
    ScrollPos( 42,  4 ),
    ScrollPos( 43,  4 ),
    ScrollPos( 44,  5 ),
    ScrollPos( 45,  5 ),
    ScrollPos( 46,  6 ),
    ScrollPos( 47,  6 ),
    ScrollPos( 48,  7 ),
    ScrollPos( 49,  7 ),
    ScrollPos( 50,  8 ),
    ScrollPos( 51,  8 ),
    ScrollPos( 52,  9 ),
    ScrollPos( 53,  9 ),
    ScrollPos( 54, 10 ),
    ScrollPos( 55, 10 ),
    ScrollPos( 56, 11 ),
    ScrollPos( 57, 11 ),
    -1,
};
static constexpr int16_t kScrollPos19[] = {
    ScrollPos( 13,  1 ),
    ScrollPos( 14,  1 ),
    ScrollPos( 15,  2 ),
    ScrollPos( 16,  2 ),
    ScrollPos( 17,  3 ),
    ScrollPos( 18,  3 ),
    ScrollPos( 19,  4 ),
    ScrollPos( 20,  4 ),
    ScrollPos( 21,  5 ),
    ScrollPos( 22,  5 ),
    ScrollPos( 23,  6 ),
    ScrollPos( 24,  6 ),
    ScrollPos( 25,  7 ),
    ScrollPos( 26,  7 ),
    ScrollPos( 27,  8 ),
    ScrollPos( 28,  8 ),
    ScrollPos( 29,  9 ),
    ScrollPos( 30,  9 ),
    ScrollPos( 31, 10 ),
    ScrollPos( 32, 10 ),
    ScrollPos( 33, 10 ),
    ScrollPos( 34,  9 ),
    ScrollPos( 35,  9 ),
    ScrollPos( 36,  8 ),
    ScrollPos( 37,  8 ),
    ScrollPos( 38,  7 ),
    ScrollPos( 39,  7 ),
    ScrollPos( 40,  6 ),
    ScrollPos( 41,  6 ),
    ScrollPos( 42,  5 ),
    ScrollPos( 43,  5 ),
    ScrollPos( 44,  4 ),
    ScrollPos( 45,  4 ),
    ScrollPos( 46,  3 ),
    ScrollPos( 47,  3 ),
    ScrollPos( 48,  2 ),
    ScrollPos( 49,  2 ),
    ScrollPos( 50,  1 ),
    ScrollPos( 51,  1 ),
    -1,
};
static constexpr int16_t kScrollPos20[] = {
    ScrollPos( 12,  1 ),
    ScrollPos( 13,  3 ),
    ScrollPos( 14,  4 ),
    ScrollPos( 15,  5 ),
    ScrollPos( 16,  6 ),
    ScrollPos( 17,  7 ),
    ScrollPos( 18,  7 ),
    ScrollPos( 19,  8 ),
    ScrollPos( 20,  8 ),
    ScrollPos( 21,  9 ),
    ScrollPos( 22,  9 ),
    ScrollPos( 23,  9 ),
    ScrollPos( 24, 10 ),
    ScrollPos( 25, 10 ),
    ScrollPos( 26, 10 ),
    ScrollPos( 27, 10 ),
    ScrollPos( 28, 10 ),
    ScrollPos( 29, 10 ),
    ScrollPos( 30, 10 ),
    ScrollPos( 31, 10 ),
    ScrollPos( 32, 10 ),
    ScrollPos( 33, 10 ),
    ScrollPos( 34, 10 ),
    ScrollPos( 35, 10 ),
    ScrollPos( 36, 10 ),
    ScrollPos( 37, 10 ),
    ScrollPos( 38, 10 ),
    ScrollPos( 39,  9 ),
    ScrollPos( 40,  9 ),
    ScrollPos( 41,  9 ),
    ScrollPos( 42,  8 ),
    ScrollPos( 43,  8 ),
    ScrollPos( 44,  7 ),
    ScrollPos( 45,  7 ),
    ScrollPos( 46,  6 ),
    ScrollPos( 47,  5 ),
    ScrollPos( 48,  4 ),
    ScrollPos( 49,  3 ),
    -1,
};
static constexpr int16_t kScrollPos21[] = {
    ScrollPos( 12,  1 ),
    ScrollPos( 13,  1 ),
    ScrollPos( 14,  2 ),
    ScrollPos( 15,  2 ),
    ScrollPos( 16,  3 ),
    ScrollPos( 17,  3 ),
    ScrollPos( 18,  4 ),
    ScrollPos( 19,  4 ),
    ScrollPos( 20,  5 ),
    ScrollPos( 21,  5 ),
    ScrollPos( 22,  6 ),
    ScrollPos( 23,  6 ),
    ScrollPos( 24,  7 ),
    ScrollPos( 25,  7 ),
    ScrollPos( 26,  8 ),
    ScrollPos( 27,  8 ),
    ScrollPos( 28,  9 ),
    ScrollPos( 29,  9 ),
    ScrollPos( 30, 10 ),
    ScrollPos( 31, 10 ),
    ScrollPos( 32, 10 ),
    ScrollPos( 33,  9 ),
    ScrollPos( 34,  9 ),
    ScrollPos( 35,  8 ),
    ScrollPos( 36,  8 ),
    ScrollPos( 37,  7 ),
    ScrollPos( 38,  7 ),
    ScrollPos( 39,  6 ),
    ScrollPos( 40,  6 ),
    ScrollPos( 41,  5 ),
    ScrollPos( 42,  5 ),
    ScrollPos( 43,  4 ),
    ScrollPos( 44,  4 ),
    ScrollPos( 45,  3 ),
    ScrollPos( 46,  3 ),
    ScrollPos( 47,  2 ),
    ScrollPos( 48,  2 ),
    ScrollPos( 49,  1 ),
    -1,
};
static constexpr int16_t kScrollPos22[] = {
    ScrollPos( 16,  1 ),
    ScrollPos( 17,  1 ),
    ScrollPos( 18,  2 ),
    ScrollPos( 19,  2 ),
    ScrollPos( 20,  3 ),
    ScrollPos( 21,  3 ),
    ScrollPos( 22,  4 ),
    ScrollPos( 23,  4 ),
    ScrollPos( 24,  5 ),
    ScrollPos( 25,  5 ),
    ScrollPos( 26,  6 ),
    ScrollPos( 27,  6 ),
    ScrollPos( 28,  6 ),
    ScrollPos( 29,  6 ),
    ScrollPos( 30,  6 ),
    ScrollPos( 31,  6 ),
    ScrollPos( 32,  6 ),
    ScrollPos( 33,  6 ),
    ScrollPos( 34,  6 ),
    ScrollPos( 35,  6 ),
    ScrollPos( 36,  6 ),
    ScrollPos( 37,  6 ),
    ScrollPos( 38,  6 ),
    ScrollPos( 39,  5 ),
    ScrollPos( 40,  5 ),
    ScrollPos( 41,  4 ),
    ScrollPos( 42,  4 ),
    ScrollPos( 43,  3 ),
    ScrollPos( 44,  3 ),
    ScrollPos( 45,  2 ),
    ScrollPos( 46,  2 ),
    ScrollPos( 47,  1 ),
    ScrollPos( 48,  1 ),
    -1,
};
static constexpr int16_t kScrollPos23[] = {
    ScrollPos( 15,  1 ),
    ScrollPos( 16,  2 ),
    ScrollPos( 17,  2 ),
    ScrollPos( 18,  3 ),
    ScrollPos( 19,  4 ),
    ScrollPos( 20,  5 ),
    ScrollPos( 21,  5 ),
    ScrollPos( 22,  5 ),
    ScrollPos( 23,  6 ),
    ScrollPos( 24,  6 ),
    ScrollPos( 25,  6 ),
    ScrollPos( 26,  6 ),
    ScrollPos( 27,  7 ),
    ScrollPos( 28,  7 ),
    ScrollPos( 29,  7 ),
    ScrollPos( 30,  7 ),
    ScrollPos( 31,  7 ),
    ScrollPos( 32,  7 ),
    ScrollPos( 33,  7 ),
    ScrollPos( 34,  7 ),
    ScrollPos( 35,  7 ),
    ScrollPos( 36,  7 ),
    ScrollPos( 37,  6 ),
    ScrollPos( 38,  6 ),
    ScrollPos( 39,  6 ),
    ScrollPos( 40,  6 ),
    ScrollPos( 41,  5 ),
    ScrollPos( 42,  5 ),
    ScrollPos( 43,  5 ),
    ScrollPos( 44,  4 ),
    ScrollPos( 45,  3 ),
    ScrollPos( 46,  3 ),
    ScrollPos( 47,  2 ),
    ScrollPos( 48,  1 ),
    -1,
};
static constexpr int16_t kScrollPos24[] = {
    ScrollPos(  8,  9 ),
    ScrollPos(  9,  9 ),
    ScrollPos( 10,  8 ),
    ScrollPos( 11,  8 ),
    ScrollPos( 12,  7 ),
    ScrollPos( 13,  7 ),
    ScrollPos( 14,  6 ),
    ScrollPos( 15,  6 ),
    ScrollPos( 16,  5 ),
    ScrollPos( 17,  5 ),
    ScrollPos( 18,  4 ),
    ScrollPos( 19,  4 ),
    ScrollPos( 20,  3 ),
    ScrollPos( 21,  3 ),
    ScrollPos( 22,  2 ),
    ScrollPos( 23,  2 ),
    ScrollPos( 24,  1 ),
    ScrollPos( 25,  1 ),
    ScrollPos( 26,  0 ),
    ScrollPos( 27,  0 ),
    -1,
};
static constexpr int16_t kScrollPos25[] = {
    ScrollPos( 36,  0 ),
    ScrollPos( 37,  0 ),
    ScrollPos( 38,  1 ),
    ScrollPos( 39,  1 ),
    ScrollPos( 40,  2 ),
    ScrollPos( 41,  2 ),
    ScrollPos( 42,  3 ),
    ScrollPos( 43,  3 ),
    ScrollPos( 44,  4 ),
    ScrollPos( 45,  4 ),
    ScrollPos( 46,  5 ),
    ScrollPos( 47,  5 ),
    ScrollPos( 48,  6 ),
    ScrollPos( 49,  6 ),
    ScrollPos( 50,  7 ),
    ScrollPos( 51,  7 ),
    ScrollPos( 52,  8 ),
    ScrollPos( 53,  8 ),
    ScrollPos( 54,  9 ),
    ScrollPos( 55,  9 ),
    -1,
};
static constexpr int16_t kScrollPos26[] = {
    ScrollPos(  4, 13 ),
    ScrollPos(  5, 13 ),
    ScrollPos(  6, 12 ),
    ScrollPos(  7, 12 ),
    ScrollPos(  8, 11 ),
    ScrollPos(  9, 11 ),
    ScrollPos( 10, 10 ),
    ScrollPos( 11, 10 ),
    ScrollPos( 12,  9 ),
    ScrollPos( 13,  9 ),
    ScrollPos( 14,  8 ),
    ScrollPos( 15,  8 ),
    ScrollPos( 16,  7 ),
    ScrollPos( 17,  7 ),
    ScrollPos( 18,  6 ),
    ScrollPos( 19,  6 ),
    ScrollPos( 20,  5 ),
    ScrollPos( 21,  5 ),
    ScrollPos( 22,  4 ),
    ScrollPos( 23,  4 ),
    ScrollPos( 24,  3 ),
    ScrollPos( 25,  3 ),
    ScrollPos( 26,  2 ),
    ScrollPos( 27,  2 ),
    ScrollPos( 28,  1 ),
    ScrollPos( 29,  1 ),
    ScrollPos( 30,  0 ),
    ScrollPos( 31,  0 ),
    -1,
};
static constexpr int16_t kScrollPos27[] = {
    ScrollPos( 32,  0 ),
    ScrollPos( 33,  0 ),
    ScrollPos( 34,  1 ),
    ScrollPos( 35,  1 ),
    ScrollPos( 36,  2 ),
    ScrollPos( 37,  2 ),
    ScrollPos( 38,  3 ),
    ScrollPos( 39,  3 ),
    ScrollPos( 40,  4 ),
    ScrollPos( 41,  4 ),
    ScrollPos( 42,  5 ),
    ScrollPos( 43,  5 ),
    ScrollPos( 44,  6 ),
    ScrollPos( 45,  6 ),
    ScrollPos( 46,  7 ),
    ScrollPos( 47,  7 ),
    ScrollPos( 48,  8 ),
    ScrollPos( 49,  8 ),
    ScrollPos( 50,  9 ),
    ScrollPos( 51,  9 ),
    ScrollPos( 52, 10 ),
    ScrollPos( 53, 10 ),
    ScrollPos( 54, 11 ),
    ScrollPos( 55, 11 ),
    ScrollPos( 56, 12 ),
    ScrollPos( 57, 12 ),
    ScrollPos( 58, 13 ),
    ScrollPos( 59, 13 ),
    -1,
};
static constexpr int16_t kScrollPos28[] = {
    ScrollPos(  6, 13 ),
    ScrollPos(  7, 13 ),
    ScrollPos(  8, 12 ),
    ScrollPos(  9, 12 ),
    ScrollPos( 10, 11 ),
    ScrollPos( 11, 11 ),
    ScrollPos( 12, 10 ),
    ScrollPos( 13, 10 ),
    ScrollPos( 14,  9 ),
    ScrollPos( 15,  9 ),
    ScrollPos( 16,  8 ),
    ScrollPos( 17,  8 ),
    ScrollPos( 18,  7 ),
    ScrollPos( 19,  7 ),
    ScrollPos( 20,  6 ),
    ScrollPos( 21,  6 ),
    ScrollPos( 22,  5 ),
    ScrollPos( 23,  5 ),
    ScrollPos( 24,  4 ),
    ScrollPos( 25,  4 ),
    ScrollPos( 26,  3 ),
    ScrollPos( 27,  3 ),
    ScrollPos( 28,  2 ),
    ScrollPos( 29,  2 ),
    ScrollPos( 30,  1 ),
    ScrollPos( 31,  1 ),
    ScrollPos( 32,  0 ),
    ScrollPos( 33,  0 ),
    -1,
};
static constexpr int16_t kScrollPos29[] = {
    ScrollPos( 30,  0 ),
    ScrollPos( 31,  0 ),
    ScrollPos( 32,  1 ),
    ScrollPos( 33,  1 ),
    ScrollPos( 34,  2 ),
    ScrollPos( 35,  2 ),
    ScrollPos( 36,  3 ),
    ScrollPos( 37,  3 ),
    ScrollPos( 38,  4 ),
    ScrollPos( 39,  4 ),
    ScrollPos( 40,  5 ),
    ScrollPos( 41,  5 ),
    ScrollPos( 42,  6 ),
    ScrollPos( 43,  6 ),
    ScrollPos( 44,  7 ),
    ScrollPos( 45,  7 ),
    ScrollPos( 46,  8 ),
    ScrollPos( 47,  8 ),
    ScrollPos( 48,  9 ),
    ScrollPos( 49,  9 ),
    ScrollPos( 50, 10 ),
    ScrollPos( 51, 10 ),
    ScrollPos( 52, 11 ),
    ScrollPos( 53, 11 ),
    ScrollPos( 54, 12 ),
    ScrollPos( 55, 12 ),
    ScrollPos( 56, 13 ),
    ScrollPos( 57, 13 ),
    -1,
};
static constexpr int16_t kScrollPos30[] = {
    ScrollPos(  2, 30 ),
    ScrollPos(  3, 30 ),
    ScrollPos(  4, 29 ),
    ScrollPos(  5, 29 ),
    ScrollPos(  6, 28 ),
    ScrollPos(  7, 28 ),
    ScrollPos(  8, 27 ),
    ScrollPos(  9, 27 ),
    ScrollPos( 10, 26 ),
    ScrollPos( 11, 26 ),
    ScrollPos( 12, 25 ),
    ScrollPos( 13, 25 ),
    ScrollPos( 14, 24 ),
    ScrollPos( 15, 24 ),
    ScrollPos( 16, 23 ),
    ScrollPos( 17, 23 ),
    ScrollPos( 18, 22 ),
    ScrollPos( 19, 22 ),
    ScrollPos( 20, 21 ),
    ScrollPos( 21, 21 ),
    ScrollPos( 22, 20 ),
    ScrollPos( 23, 20 ),
    ScrollPos( 24, 19 ),
    ScrollPos( 25, 19 ),
    ScrollPos( 26, 18 ),
    ScrollPos( 27, 18 ),
    ScrollPos( 28, 17 ),
    ScrollPos( 29, 17 ),
    ScrollPos( 30, 16 ),
    ScrollPos( 31, 16 ),
    ScrollPos( 32, 15 ),
    ScrollPos( 33, 15 ),
    ScrollPos( 34, 14 ),
    ScrollPos( 35, 14 ),
    ScrollPos( 36, 13 ),
    ScrollPos( 37, 13 ),
    ScrollPos( 38, 12 ),
    ScrollPos( 39, 12 ),
    ScrollPos( 40, 11 ),
    ScrollPos( 41, 11 ),
    ScrollPos( 42, 10 ),
    ScrollPos( 43, 10 ),
    ScrollPos( 44,  9 ),
    ScrollPos( 45,  9 ),
    ScrollPos( 46,  8 ),
    ScrollPos( 47,  8 ),
    ScrollPos( 48,  7 ),
    ScrollPos( 49,  7 ),
    ScrollPos( 50,  6 ),
    ScrollPos( 51,  6 ),
    ScrollPos( 52,  5 ),
    ScrollPos( 53,  5 ),
    ScrollPos( 54,  4 ),
    ScrollPos( 55,  4 ),
    ScrollPos( 56,  3 ),
    ScrollPos( 57,  3 ),
    ScrollPos( 58,  2 ),
    ScrollPos( 59,  2 ),
    ScrollPos( 60,  1 ),
    ScrollPos( 61,  1 ),
    ScrollPos( 62,  0 ),
    -1,
};
static constexpr int16_t kScrollPos31[] = {
    ScrollPos(  1,  0 ),
    ScrollPos(  2,  1 ),
    ScrollPos(  3,  1 ),
    ScrollPos(  4,  2 ),
    ScrollPos(  5,  2 ),
    ScrollPos(  6,  3 ),
    ScrollPos(  7,  3 ),
    ScrollPos(  8,  4 ),
    ScrollPos(  9,  4 ),
    ScrollPos( 10,  5 ),
    ScrollPos( 11,  5 ),
    ScrollPos( 12,  6 ),
    ScrollPos( 13,  6 ),
    ScrollPos( 14,  7 ),
    ScrollPos( 15,  7 ),
    ScrollPos( 16,  8 ),
    ScrollPos( 17,  8 ),
    ScrollPos( 18,  9 ),
    ScrollPos( 19,  9 ),
    ScrollPos( 20, 10 ),
    ScrollPos( 21, 10 ),
    ScrollPos( 22, 11 ),
    ScrollPos( 23, 11 ),
    ScrollPos( 24, 12 ),
    ScrollPos( 25, 12 ),
    ScrollPos( 26, 13 ),
    ScrollPos( 27, 13 ),
    ScrollPos( 28, 14 ),
    ScrollPos( 29, 14 ),
    ScrollPos( 30, 15 ),
    ScrollPos( 31, 15 ),
    ScrollPos( 32, 16 ),
    ScrollPos( 33, 16 ),
    ScrollPos( 34, 17 ),
    ScrollPos( 35, 17 ),
    ScrollPos( 36, 18 ),
    ScrollPos( 37, 18 ),
    ScrollPos( 38, 19 ),
    ScrollPos( 39, 19 ),
    ScrollPos( 40, 20 ),
    ScrollPos( 41, 20 ),
    ScrollPos( 42, 21 ),
    ScrollPos( 43, 21 ),
    ScrollPos( 44, 22 ),
    ScrollPos( 45, 22 ),
    ScrollPos( 46, 23 ),
    ScrollPos( 47, 23 ),
    ScrollPos( 48, 24 ),
    ScrollPos( 49, 24 ),
    ScrollPos( 50, 25 ),
    ScrollPos( 51, 25 ),
    ScrollPos( 52, 26 ),
    ScrollPos( 53, 26 ),
    ScrollPos( 54, 27 ),
    ScrollPos( 55, 27 ),
    ScrollPos( 56, 28 ),
    ScrollPos( 57, 28 ),
    ScrollPos( 58, 29 ),
    ScrollPos( 59, 29 ),
    ScrollPos( 60, 30 ),
    ScrollPos( 61, 30 ),
    -1,
};
static constexpr int16_t kScrollPos32[] = {
    ScrollPos( 12,  0 ),
    ScrollPos( 13,  1 ),
    ScrollPos( 14,  1 ),
    ScrollPos( 15,  2 ),
    ScrollPos( 16,  2 ),
    ScrollPos( 17,  3 ),
    ScrollPos( 18,  3 ),
    ScrollPos( 19,  4 ),
    ScrollPos( 20,  4 ),
    ScrollPos( 21,  5 ),
    ScrollPos( 22,  5 ),
    ScrollPos( 23,  6 ),
    ScrollPos( 24,  6 ),
    ScrollPos( 25,  7 ),
    ScrollPos( 26,  7 ),
    ScrollPos( 27,  8 ),
    ScrollPos( 28,  8 ),
    ScrollPos( 29,  9 ),
    ScrollPos( 30,  9 ),
    ScrollPos( 31, 10 ),
    ScrollPos( 32, 10 ),
    ScrollPos( 33, 11 ),
    ScrollPos( 34, 11 ),
    ScrollPos( 35, 12 ),
    ScrollPos( 36, 12 ),
    ScrollPos( 37, 13 ),
    ScrollPos( 38, 13 ),
    ScrollPos( 39, 14 ),
    ScrollPos( 40, 14 ),
    ScrollPos( 41, 15 ),
    ScrollPos( 42, 15 ),
    ScrollPos( 43, 16 ),
    ScrollPos( 44, 16 ),
    ScrollPos( 45, 17 ),
    ScrollPos( 46, 17 ),
    ScrollPos( 47, 18 ),
    ScrollPos( 48, 18 ),
    ScrollPos( 49, 19 ),
    ScrollPos( 50, 19 ),
    -1,
};
static constexpr int16_t kScrollPos33[] = {
    ScrollPos( 12, 20 ),
    ScrollPos( 13, 20 ),
    ScrollPos( 14, 19 ),
    ScrollPos( 15, 19 ),
    ScrollPos( 16, 18 ),
    ScrollPos( 17, 18 ),
    ScrollPos( 18, 17 ),
    ScrollPos( 19, 17 ),
    ScrollPos( 20, 16 ),
    ScrollPos( 21, 16 ),
    ScrollPos( 22, 15 ),
    ScrollPos( 23, 15 ),
    ScrollPos( 24, 14 ),
    ScrollPos( 25, 14 ),
    ScrollPos( 26, 13 ),
    ScrollPos( 27, 13 ),
    ScrollPos( 28, 12 ),
    ScrollPos( 29, 12 ),
    ScrollPos( 30, 11 ),
    ScrollPos( 31, 11 ),
    ScrollPos( 32, 10 ),
    ScrollPos( 33, 10 ),
    ScrollPos( 34,  9 ),
    ScrollPos( 35,  9 ),
    ScrollPos( 36,  8 ),
    ScrollPos( 37,  8 ),
    ScrollPos( 38,  7 ),
    ScrollPos( 39,  7 ),
    ScrollPos( 40,  6 ),
    ScrollPos( 41,  6 ),
    ScrollPos( 42,  5 ),
    ScrollPos( 43,  5 ),
    ScrollPos( 44,  4 ),
    ScrollPos( 45,  4 ),
    ScrollPos( 46,  3 ),
    ScrollPos( 47,  3 ),
    ScrollPos( 48,  2 ),
    ScrollPos( 49,  2 ),
    ScrollPos( 50,  1 ),
    ScrollPos( 51,  1 ),
    -1,
};
static constexpr int16_t kScrollPos34[] = {
    ScrollPos(  2, 14 ),
    ScrollPos(  3, 14 ),
    ScrollPos(  4, 13 ),
    ScrollPos(  5, 13 ),
    ScrollPos(  6, 12 ),
    ScrollPos(  7, 12 ),
    ScrollPos(  8, 11 ),
    ScrollPos(  9, 11 ),
    ScrollPos( 10, 10 ),
    ScrollPos( 11, 10 ),
    ScrollPos( 12,  9 ),
    ScrollPos( 13,  9 ),
    ScrollPos( 14,  8 ),
    ScrollPos( 15,  8 ),
    ScrollPos( 16,  7 ),
    ScrollPos( 17,  7 ),
    ScrollPos( 18,  6 ),
    ScrollPos( 19,  6 ),
    ScrollPos( 20,  5 ),
    ScrollPos( 21,  5 ),
    ScrollPos( 22,  4 ),
    ScrollPos( 23,  4 ),
    ScrollPos( 24,  3 ),
    ScrollPos( 25,  3 ),
    ScrollPos( 26,  2 ),
    ScrollPos( 27,  2 ),
    ScrollPos( 28,  1 ),
    ScrollPos( 29,  1 ),
    ScrollPos( 30,  0 ),
    -1,
};
static constexpr int16_t kScrollPos35[] = {
    ScrollPos( 33,  0 ),
    ScrollPos( 34,  0 ),
    ScrollPos( 35,  1 ),
    ScrollPos( 36,  1 ),
    ScrollPos( 37,  2 ),
    ScrollPos( 38,  2 ),
    ScrollPos( 39,  3 ),
    ScrollPos( 40,  3 ),
    ScrollPos( 41,  4 ),
    ScrollPos( 42,  4 ),
    ScrollPos( 43,  5 ),
    ScrollPos( 44,  5 ),
    ScrollPos( 45,  6 ),
    ScrollPos( 46,  6 ),
    ScrollPos( 47,  7 ),
    ScrollPos( 48,  7 ),
    ScrollPos( 49,  8 ),
    ScrollPos( 50,  8 ),
    ScrollPos( 51,  9 ),
    ScrollPos( 52,  9 ),
    ScrollPos( 53, 10 ),
    ScrollPos( 54, 10 ),
    ScrollPos( 55, 11 ),
    ScrollPos( 56, 11 ),
    ScrollPos( 57, 12 ),
    ScrollPos( 58, 12 ),
    ScrollPos( 59, 13 ),
    ScrollPos( 60, 13 ),
    ScrollPos( 61, 14 ),
    -1,
};
static constexpr int16_t kScrollPos36[] = {
    ScrollPos(  4,  0 ),
    ScrollPos(  5,  1 ),
    ScrollPos(  6,  2 ),
    ScrollPos(  7,  3 ),
    ScrollPos(  8,  3 ),
    ScrollPos(  9,  4 ),
    ScrollPos( 10,  5 ),
    ScrollPos( 11,  5 ),
    ScrollPos( 12,  6 ),
    ScrollPos( 13,  6 ),
    ScrollPos( 14,  7 ),
    ScrollPos( 15,  7 ),
    ScrollPos( 16,  8 ),
    ScrollPos( 17,  8 ),
    ScrollPos( 18,  9 ),
    ScrollPos( 19,  9 ),
    ScrollPos( 20, 10 ),
    ScrollPos( 21, 10 ),
    ScrollPos( 22, 10 ),
    ScrollPos( 23, 11 ),
    ScrollPos( 24, 11 ),
    ScrollPos( 25, 11 ),
    ScrollPos( 26, 12 ),
    ScrollPos( 27, 12 ),
    ScrollPos( 28, 12 ),
    ScrollPos( 29, 12 ),
    ScrollPos( 30, 12 ),
    -1,
};
static constexpr int16_t kScrollPos37[] = {
    ScrollPos( 32, 13 ),
    ScrollPos( 33, 12 ),
    ScrollPos( 34, 12 ),
    ScrollPos( 35, 12 ),
    ScrollPos( 36, 12 ),
    ScrollPos( 37, 11 ),
    ScrollPos( 38, 11 ),
    ScrollPos( 39, 11 ),
    ScrollPos( 40, 10 ),
    ScrollPos( 41, 10 ),
    ScrollPos( 42, 10 ),
    ScrollPos( 43,  9 ),
    ScrollPos( 44,  9 ),
    ScrollPos( 45,  8 ),
    ScrollPos( 46,  8 ),
    ScrollPos( 47,  7 ),
    ScrollPos( 48,  7 ),
    ScrollPos( 49,  6 ),
    ScrollPos( 50,  6 ),
    ScrollPos( 51,  5 ),
    ScrollPos( 52,  5 ),
    ScrollPos( 53,  4 ),
    ScrollPos( 54,  3 ),
    ScrollPos( 55,  3 ),
    ScrollPos( 56,  2 ),
    ScrollPos( 57,  1 ),
    ScrollPos( 58,  0 ),
    -1,
};

static constexpr const int16_t* kScrollPositions[kMaxScrollingTextModes] = {
    kScrollPos0,
    kScrollPos1,
    kScrollPos2,
    kScrollPos3,
    kScrollPos4,
    kScrollPos5,
    kScrollPos6,
    kScrollPos7,
    kScrollPos8,
    kScrollPos9,
    kScrollPos10,
    kScrollPos11,
    kScrollPos12,
    kScrollPos13,
    kScrollPos14,
    kScrollPos15,
    kScrollPos16,
    kScrollPos17,
    kScrollPos18,
    kScrollPos19,
    kScrollPos20,
    kScrollPos21,
    kScrollPos22,
    kScrollPos23,
    kScrollPos24,
    kScrollPos25,
    kScrollPos26,
    kScrollPos27,
    kScrollPos28,
    kScrollPos29,
    kScrollPos30,
    kScrollPos31,
    kScrollPos32,
    kScrollPos33,
    kScrollPos34,
    kScrollPos35,
    kScrollPos36,
    kScrollPos37,
};
// clang-format on

void ScrollingTextInvalidate()
{
    for (auto& scrollText : _drawScrollTextList)
    {
        scrollText.string_id = 0;
        std::memset(scrollText.string_args, 0, sizeof(scrollText.string_args));
    }
}

ImageId ScrollingTextSetup(
    PaintSession& session, StringId stringId, Formatter& ft, uint16_t scroll, uint16_t scrollingMode, colour_t colour)
{
    std::scoped_lock<std::mutex> lock(_scrollingTextMutex);

    assert(scrollingMode < kMaxScrollingTextModes);

    if (session.DPI.zoom_level > ZoomLevel{ 0 })
        return ImageId(SPR_SCROLLING_TEXT_DEFAULT);

    _drawSCrollNextIndex++;
    ft.Rewind();
    uint32_t scrollIndex = ScrollingTextGetMatchingOrOldest(stringId, ft, scroll, scrollingMode, colour);
    if (scrollIndex >= SPR_SCROLLING_TEXT_START)
        return ImageId(scrollIndex);

    // Setup scrolling text
    auto scrollText = &_drawScrollTextList[scrollIndex];
    scrollText->string_id = stringId;
    std::memcpy(scrollText->string_args, ft.Buf(), sizeof(scrollText->string_args));
    scrollText->colour = colour;
    scrollText->position = scroll;
    scrollText->mode = scrollingMode;
    scrollText->id = _drawSCrollNextIndex;

    // Create the string to draw
    utf8 scrollString[256];
    ScrollingTextFormat(scrollString, 256, scrollText);

    const int16_t* scrollingModePositions = kScrollPositions[scrollingMode];

    std::fill_n(scrollText->bitmap, 320 * 8, 0x00);
    if (LocalisationService_UseTrueTypeFont())
    {
        ScrollingTextSetBitmapForTTF(scrollString, scroll, scrollText->bitmap, scrollingModePositions, colour);
    }
    else
    {
        ScrollingTextSetBitmapForSprite(scrollString, scroll, scrollText->bitmap, scrollingModePositions, colour);
    }

    uint32_t imageId = SPR_SCROLLING_TEXT_START + scrollIndex;
    DrawingEngineInvalidateImage(imageId);
    return ImageId(imageId);
}

static void ScrollingTextSetBitmapForSprite(
    std::string_view text, int32_t scroll, uint8_t* bitmap, const int16_t* scrollPositionOffsets, colour_t colour)
{
    auto characterColour = colour;
    auto fmt = FmtString(text);

    // Repeat string a maximum of four times (eliminates possibility of infinite loop)
    for (auto i = 0; i < 4; i++)
    {
        for (const auto& token : fmt)
        {
            if (token.IsLiteral())
            {
                CodepointView codepoints(token.text);
                for (auto codepoint : codepoints)
                {
                    auto characterWidth = FontSpriteGetCodepointWidth(FontStyle::Tiny, codepoint);
                    auto characterBitmap = FontSpriteGetCodepointBitmap(codepoint);
                    for (; characterWidth != 0; characterWidth--, characterBitmap++)
                    {
                        // Skip any non-displayed columns
                        if (scroll != 0)
                        {
                            scroll--;
                            continue;
                        }

                        int16_t scrollPosition = *scrollPositionOffsets;
                        if (scrollPosition == -1)
                            return;

                        if (scrollPosition > -1)
                        {
                            auto dst = &bitmap[scrollPosition];
                            for (uint8_t char_bitmap = *characterBitmap; char_bitmap != 0; char_bitmap >>= 1)
                            {
                                if (char_bitmap & 1)
                                    *dst = characterColour;

                                // Jump to next row
                                dst += 64;
                            }
                        }
                        scrollPositionOffsets++;
                    }
                }
            }
            else if (FormatTokenIsColour(token.kind))
            {
                auto g1 = GfxGetG1Element(SPR_TEXT_PALETTE);
                if (g1 != nullptr)
                {
                    auto colourIndex = FormatTokenGetTextColourIndex(token.kind);
                    characterColour = g1->offset[colourIndex * 4];
                }
            }
        }
    }
}

static void ScrollingTextSetBitmapForTTF(
    std::string_view text, int32_t scroll, uint8_t* bitmap, const int16_t* scrollPositionOffsets, colour_t colour)
{
#ifndef NO_TTF
    auto fontDesc = TTFGetFontFromSpriteBase(FontStyle::Tiny);
    if (fontDesc->font == nullptr)
    {
        ScrollingTextSetBitmapForSprite(text, scroll, bitmap, scrollPositionOffsets, colour);
        return;
    }

    thread_local std::string ttfBuffer;
    ttfBuffer.clear();

    auto fmt = FmtString(text);
    for (const auto& token : fmt)
    {
        if (token.IsLiteral())
        {
            ttfBuffer.append(token.text);
        }
        else if (FormatTokenIsColour(token.kind))
        {
            auto g1 = GfxGetG1Element(SPR_TEXT_PALETTE);
            if (g1 != nullptr)
            {
                auto colourIndex = FormatTokenGetTextColourIndex(token.kind);
                colour = g1->offset[colourIndex * 4];
            }
        }
    }

    auto surface = TTFSurfaceCacheGetOrAdd(fontDesc->font, ttfBuffer.c_str());
    if (surface == nullptr)
    {
        return;
    }

    int32_t width = surface->w;
    auto src = static_cast<const uint8_t*>(surface->pixels);

    // Pitch offset
    src += 2 * width;

    // Line height offset
    int32_t min_vpos = -fontDesc->offset_y;
    int32_t max_vpos = std::min(surface->h - 2, min_vpos + 7);

    bool use_hinting = Config::Get().fonts.EnableHinting && fontDesc->hinting_threshold > 0;

    for (int32_t x = 0;; x++)
    {
        if (x >= width)
            x = 0;

        // Skip any non-displayed columns
        if (scroll == 0)
        {
            int16_t scrollPosition = *scrollPositionOffsets;
            if (scrollPosition == -1)
                return;

            if (scrollPosition > -1)
            {
                uint8_t* dst = &bitmap[scrollPosition];

                for (int32_t y = min_vpos; y < max_vpos; y++)
                {
                    uint8_t src_pixel = src[y * width + x];
                    if ((!use_hinting && src_pixel != 0) || src_pixel > 140)
                    {
                        // Centre of the glyph: use full colour.
                        *dst = colour;
                    }
                    else if (use_hinting && src_pixel > fontDesc->hinting_threshold)
                    {
                        // Simulate font hinting by shading the background colour instead.
                        *dst = BlendColours(colour, *dst);
                    }

                    // Jump to next row
                    dst += 64;
                }
            }
            scrollPositionOffsets++;
        }
        else
        {
            scroll--;
        }
    }
#endif // NO_TTF
}
