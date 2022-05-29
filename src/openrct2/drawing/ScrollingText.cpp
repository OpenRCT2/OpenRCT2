/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScrollingText.h"

#include "../Context.h"
#include "../config/Config.h"
#include "../core/String.hpp"
#include "../interface/Colour.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../localisation/Localisation.h"
#include "../localisation/LocalisationService.h"
#include "../paint/Paint.h"
#include "../sprites.h"
#include "Drawing.h"
#include "TTF.h"

#include <algorithm>
#include <mutex>

using namespace OpenRCT2;

struct rct_draw_scroll_text
{
    rct_string_id string_id;
    uint8_t string_args[32];
    colour_t colour;
    uint16_t position;
    uint16_t mode;
    uint32_t id;
    uint8_t bitmap[64 * 40];
};

static rct_draw_scroll_text _drawScrollTextList[OpenRCT2::MaxScrollingTextEntries];
static uint8_t _characterBitmaps[FONT_SPRITE_GLYPH_COUNT + SPR_G2_GLYPH_COUNT][8];
static uint32_t _drawSCrollNextIndex = 0;
static std::mutex _scrollingTextMutex;

static void scrolling_text_set_bitmap_for_sprite(
    std::string_view text, int32_t scroll, uint8_t* bitmap, const int16_t* scrollPositionOffsets, colour_t colour);
static void scrolling_text_set_bitmap_for_ttf(
    std::string_view text, int32_t scroll, uint8_t* bitmap, const int16_t* scrollPositionOffsets, colour_t colour);

void scrolling_text_initialise_bitmaps()
{
    uint8_t drawingSurface[64];
    rct_drawpixelinfo dpi;
    dpi.bits = reinterpret_cast<uint8_t*>(&drawingSurface);
    dpi.width = 8;
    dpi.height = 8;

    for (int32_t i = 0; i < FONT_SPRITE_GLYPH_COUNT; i++)
    {
        std::fill_n(drawingSurface, sizeof(drawingSurface), 0x00);
        gfx_draw_sprite_software(
            &dpi, ImageId::FromUInt32(SPR_CHAR_START + static_cast<uint32_t>(FontSpriteBase::TINY) + i), { -1, 0 });

        for (int32_t x = 0; x < 8; x++)
        {
            uint8_t val = 0;
            for (int32_t y = 0; y < 8; y++)
            {
                val >>= 1;
                uint8_t pixel = dpi.bits[x + y * 8];
                if (pixel == 1 || (gTinyFontAntiAliased && pixel == 2))
                {
                    val |= 0x80;
                }
            }
            _characterBitmaps[i][x] = val;
        }
    }

    for (int32_t i = 0; i < SPR_G2_GLYPH_COUNT; i++)
    {
        std::fill_n(drawingSurface, sizeof(drawingSurface), 0x00);
        gfx_draw_sprite_software(
            &dpi, ImageId::FromUInt32(SPR_G2_CHAR_BEGIN + (FONT_SIZE_TINY * SPR_G2_GLYPH_COUNT) + i), { -1, 0 });

        for (int32_t x = 0; x < 8; x++)
        {
            uint8_t val = 0;
            for (int32_t y = 0; y < 8; y++)
            {
                val >>= 1;
                uint8_t pixel = dpi.bits[x + y * 8];
                if (pixel == 1 || (gTinyFontAntiAliased && pixel == 2))
                {
                    val |= 0x80;
                }
            }
            _characterBitmaps[FONT_SPRITE_GLYPH_COUNT + i][x] = val;
        }
    }

    for (int32_t i = 0; i < OpenRCT2::MaxScrollingTextEntries; i++)
    {
        const int32_t imageId = SPR_SCROLLING_TEXT_START + i;

        // Initialize the scrolling text sprite.
        rct_g1_element g1{};
        g1.offset = _drawScrollTextList[i].bitmap;
        g1.x_offset = -32;
        g1.y_offset = 0;
        g1.flags = G1_FLAG_BMP;
        g1.width = 64;
        g1.height = 40;
        g1.offset[0] = 0xFF;
        g1.offset[1] = 0xFF;
        g1.offset[14] = 0;
        g1.offset[15] = 0;
        g1.offset[16] = 0;
        g1.offset[17] = 0;

        gfx_set_g1_element(imageId, &g1);
    }
}

static uint8_t* font_sprite_get_codepoint_bitmap(int32_t codepoint)
{
    auto offset = font_sprite_get_codepoint_offset(codepoint);
    if (offset >= FONT_SPRITE_GLYPH_COUNT)
    {
        return _characterBitmaps[offset - (SPR_G2_CHAR_BEGIN - SPR_CHAR_START) + FONT_SPRITE_GLYPH_COUNT];
    }

    return _characterBitmaps[offset];
}

static int32_t scrolling_text_get_matching_or_oldest(
    rct_string_id stringId, Formatter& ft, uint16_t scroll, uint16_t scrollingMode, colour_t colour)
{
    uint32_t oldestId = 0xFFFFFFFF;
    int32_t scrollIndex = -1;
    for (size_t i = 0; i < std::size(_drawScrollTextList); i++)
    {
        rct_draw_scroll_text* scrollText = &_drawScrollTextList[i];
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

static void scrolling_text_format(utf8* dst, size_t size, rct_draw_scroll_text* scrollText)
{
    if (gConfigGeneral.upper_case_banners)
    {
        format_string_to_upper(dst, size, scrollText->string_id, scrollText->string_args);
    }
    else
    {
        format_string(dst, size, scrollText->string_id, scrollText->string_args);
    }
}

extern bool TempForScrollText;

#define SCROLL_POS(x, y) (((y)*64) + (x))

// clang-format off
static constexpr const int16_t _scrollpos0[] = {
    SCROLL_POS( 35, 12 ),
    SCROLL_POS( 36, 12 ),
    SCROLL_POS( 37, 11 ),
    SCROLL_POS( 38, 11 ),
    SCROLL_POS( 39, 10 ),
    SCROLL_POS( 40, 10 ),
    SCROLL_POS( 41,  9 ),
    SCROLL_POS( 42,  9 ),
    SCROLL_POS( 43,  8 ),
    SCROLL_POS( 44,  8 ),
    SCROLL_POS( 45,  7 ),
    SCROLL_POS( 46,  7 ),
    SCROLL_POS( 47,  6 ),
    SCROLL_POS( 48,  6 ),
    SCROLL_POS( 49,  5 ),
    SCROLL_POS( 50,  5 ),
    SCROLL_POS( 51,  4 ),
    SCROLL_POS( 52,  4 ),
    SCROLL_POS( 53,  3 ),
    SCROLL_POS( 54,  3 ),
    SCROLL_POS( 55,  2 ),
    SCROLL_POS( 56,  2 ),
    SCROLL_POS( 57,  1 ),
    SCROLL_POS( 58,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos1[] = {
    SCROLL_POS(  5,  1 ),
    SCROLL_POS(  6,  1 ),
    SCROLL_POS(  7,  2 ),
    SCROLL_POS(  8,  2 ),
    SCROLL_POS(  9,  3 ),
    SCROLL_POS( 10,  3 ),
    SCROLL_POS( 11,  4 ),
    SCROLL_POS( 12,  4 ),
    SCROLL_POS( 13,  5 ),
    SCROLL_POS( 14,  5 ),
    SCROLL_POS( 15,  6 ),
    SCROLL_POS( 16,  6 ),
    SCROLL_POS( 17,  7 ),
    SCROLL_POS( 18,  7 ),
    SCROLL_POS( 19,  8 ),
    SCROLL_POS( 20,  8 ),
    SCROLL_POS( 21,  9 ),
    SCROLL_POS( 22,  9 ),
    SCROLL_POS( 23, 10 ),
    SCROLL_POS( 24, 10 ),
    SCROLL_POS( 25, 11 ),
    SCROLL_POS( 26, 11 ),
    SCROLL_POS( 27, 12 ),
    SCROLL_POS( 28, 12 ),
    -1,
};
static constexpr const int16_t _scrollpos2[] = {
    SCROLL_POS( 12,  1 ),
    SCROLL_POS( 13,  1 ),
    SCROLL_POS( 14,  2 ),
    SCROLL_POS( 15,  2 ),
    SCROLL_POS( 16,  3 ),
    SCROLL_POS( 17,  3 ),
    SCROLL_POS( 18,  4 ),
    SCROLL_POS( 19,  4 ),
    SCROLL_POS( 20,  5 ),
    SCROLL_POS( 21,  5 ),
    SCROLL_POS( 22,  6 ),
    SCROLL_POS( 23,  6 ),
    SCROLL_POS( 24,  7 ),
    SCROLL_POS( 25,  7 ),
    SCROLL_POS( 26,  8 ),
    SCROLL_POS( 27,  8 ),
    SCROLL_POS( 28,  9 ),
    SCROLL_POS( 29,  9 ),
    SCROLL_POS( 30, 10 ),
    SCROLL_POS( 31, 10 ),
    SCROLL_POS( 32, 10 ),
    SCROLL_POS( 33,  9 ),
    SCROLL_POS( 34,  9 ),
    SCROLL_POS( 35,  8 ),
    SCROLL_POS( 36,  8 ),
    SCROLL_POS( 37,  7 ),
    SCROLL_POS( 38,  7 ),
    SCROLL_POS( 39,  6 ),
    SCROLL_POS( 40,  6 ),
    SCROLL_POS( 41,  5 ),
    SCROLL_POS( 42,  5 ),
    SCROLL_POS( 43,  4 ),
    SCROLL_POS( 44,  4 ),
    SCROLL_POS( 45,  3 ),
    SCROLL_POS( 46,  3 ),
    SCROLL_POS( 47,  2 ),
    SCROLL_POS( 48,  2 ),
    SCROLL_POS( 49,  1 ),
    SCROLL_POS( 50,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos3[] = {
    SCROLL_POS( 16,  0 ),
    SCROLL_POS( 17,  1 ),
    SCROLL_POS( 18,  1 ),
    SCROLL_POS( 19,  2 ),
    SCROLL_POS( 20,  3 ),
    SCROLL_POS( 21,  3 ),
    SCROLL_POS( 22,  4 ),
    SCROLL_POS( 23,  5 ),
    SCROLL_POS( 24,  5 ),
    SCROLL_POS( 25,  6 ),
    SCROLL_POS( 26,  6 ),
    SCROLL_POS( 27,  7 ),
    SCROLL_POS( 28,  7 ),
    SCROLL_POS( 29,  8 ),
    SCROLL_POS( 30,  8 ),
    SCROLL_POS( 31,  9 ),
    SCROLL_POS( 32,  9 ),
    SCROLL_POS( 33, 10 ),
    SCROLL_POS( 34, 10 ),
    SCROLL_POS( 35, 11 ),
    SCROLL_POS( 36, 11 ),
    SCROLL_POS( 37, 12 ),
    SCROLL_POS( 38, 12 ),
    SCROLL_POS( 39, 13 ),
    SCROLL_POS( 40, 13 ),
    SCROLL_POS( 41, 14 ),
    SCROLL_POS( 42, 14 ),
    SCROLL_POS( 43, 14 ),
    SCROLL_POS( 44, 15 ),
    SCROLL_POS( 45, 15 ),
    SCROLL_POS( 46, 15 ),
    SCROLL_POS( 47, 16 ),
    SCROLL_POS( 48, 16 ),
    -1,
};
static constexpr const int16_t _scrollpos4[] = {
    SCROLL_POS( 15, 17 ),
    SCROLL_POS( 16, 17 ),
    SCROLL_POS( 17, 16 ),
    SCROLL_POS( 18, 16 ),
    SCROLL_POS( 19, 16 ),
    SCROLL_POS( 20, 15 ),
    SCROLL_POS( 21, 15 ),
    SCROLL_POS( 22, 15 ),
    SCROLL_POS( 23, 14 ),
    SCROLL_POS( 24, 14 ),
    SCROLL_POS( 25, 13 ),
    SCROLL_POS( 26, 13 ),
    SCROLL_POS( 27, 12 ),
    SCROLL_POS( 28, 12 ),
    SCROLL_POS( 29, 11 ),
    SCROLL_POS( 30, 11 ),
    SCROLL_POS( 31, 10 ),
    SCROLL_POS( 32, 10 ),
    SCROLL_POS( 33,  9 ),
    SCROLL_POS( 34,  9 ),
    SCROLL_POS( 35,  8 ),
    SCROLL_POS( 36,  8 ),
    SCROLL_POS( 37,  7 ),
    SCROLL_POS( 38,  7 ),
    SCROLL_POS( 39,  6 ),
    SCROLL_POS( 40,  6 ),
    SCROLL_POS( 41,  5 ),
    SCROLL_POS( 42,  4 ),
    SCROLL_POS( 43,  4 ),
    SCROLL_POS( 44,  3 ),
    SCROLL_POS( 45,  2 ),
    SCROLL_POS( 46,  2 ),
    SCROLL_POS( 47,  1 ),
    SCROLL_POS( 48,  0 ),
    -1,
};
static constexpr const int16_t _scrollpos5[] = {
    SCROLL_POS(  4, 12 ),
    SCROLL_POS(  5, 12 ),
    SCROLL_POS(  6, 11 ),
    SCROLL_POS(  7, 11 ),
    SCROLL_POS(  8, 10 ),
    SCROLL_POS(  9, 10 ),
    SCROLL_POS( 10,  9 ),
    SCROLL_POS( 11,  9 ),
    SCROLL_POS( 12,  8 ),
    SCROLL_POS( 13,  8 ),
    SCROLL_POS( 14,  7 ),
    SCROLL_POS( 15,  7 ),
    SCROLL_POS( 16,  6 ),
    SCROLL_POS( 17,  6 ),
    SCROLL_POS( 18,  5 ),
    SCROLL_POS( 19,  5 ),
    SCROLL_POS( 20,  4 ),
    SCROLL_POS( 21,  4 ),
    SCROLL_POS( 22,  3 ),
    SCROLL_POS( 23,  3 ),
    SCROLL_POS( 24,  2 ),
    SCROLL_POS( 25,  2 ),
    SCROLL_POS( 26,  1 ),
    SCROLL_POS( 27,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos6[] = {
    SCROLL_POS( 36,  1 ),
    SCROLL_POS( 37,  1 ),
    SCROLL_POS( 38,  2 ),
    SCROLL_POS( 39,  2 ),
    SCROLL_POS( 40,  3 ),
    SCROLL_POS( 41,  3 ),
    SCROLL_POS( 42,  4 ),
    SCROLL_POS( 43,  4 ),
    SCROLL_POS( 44,  5 ),
    SCROLL_POS( 45,  5 ),
    SCROLL_POS( 46,  6 ),
    SCROLL_POS( 47,  6 ),
    SCROLL_POS( 48,  7 ),
    SCROLL_POS( 49,  7 ),
    SCROLL_POS( 50,  8 ),
    SCROLL_POS( 51,  8 ),
    SCROLL_POS( 52,  9 ),
    SCROLL_POS( 53,  9 ),
    SCROLL_POS( 54, 10 ),
    SCROLL_POS( 55, 10 ),
    SCROLL_POS( 56, 11 ),
    SCROLL_POS( 57, 11 ),
    SCROLL_POS( 58, 12 ),
    SCROLL_POS( 59, 12 ),
    -1,
};
static constexpr const int16_t _scrollpos7[] = {
    SCROLL_POS(  8, 11 ),
    SCROLL_POS(  9, 11 ),
    SCROLL_POS( 10, 10 ),
    SCROLL_POS( 11, 10 ),
    SCROLL_POS( 12,  9 ),
    SCROLL_POS( 13,  9 ),
    SCROLL_POS( 14,  8 ),
    SCROLL_POS( 15,  8 ),
    SCROLL_POS( 16,  7 ),
    SCROLL_POS( 17,  7 ),
    SCROLL_POS( 18,  6 ),
    SCROLL_POS( 19,  6 ),
    SCROLL_POS( 20,  5 ),
    SCROLL_POS( 21,  5 ),
    SCROLL_POS( 22,  4 ),
    SCROLL_POS( 23,  4 ),
    SCROLL_POS( 24,  3 ),
    SCROLL_POS( 25,  3 ),
    SCROLL_POS( 26,  2 ),
    SCROLL_POS( 27,  2 ),
    SCROLL_POS( 28,  1 ),
    SCROLL_POS( 29,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos8[] = {
    SCROLL_POS( 36,  2 ),
    SCROLL_POS( 37,  2 ),
    SCROLL_POS( 38,  3 ),
    SCROLL_POS( 39,  3 ),
    SCROLL_POS( 40,  4 ),
    SCROLL_POS( 41,  4 ),
    SCROLL_POS( 42,  5 ),
    SCROLL_POS( 43,  5 ),
    SCROLL_POS( 44,  6 ),
    SCROLL_POS( 45,  6 ),
    SCROLL_POS( 46,  7 ),
    SCROLL_POS( 47,  7 ),
    SCROLL_POS( 48,  8 ),
    SCROLL_POS( 49,  8 ),
    SCROLL_POS( 50,  9 ),
    SCROLL_POS( 51,  9 ),
    SCROLL_POS( 52, 10 ),
    SCROLL_POS( 53, 10 ),
    SCROLL_POS( 54, 11 ),
    SCROLL_POS( 55, 11 ),
    -1,
};
static constexpr const int16_t _scrollpos9[] = {
    SCROLL_POS( 11,  9 ),
    SCROLL_POS( 12,  9 ),
    SCROLL_POS( 13,  9 ),
    SCROLL_POS( 14,  9 ),
    SCROLL_POS( 15,  9 ),
    SCROLL_POS( 16,  8 ),
    SCROLL_POS( 17,  8 ),
    SCROLL_POS( 18,  7 ),
    SCROLL_POS( 19,  7 ),
    SCROLL_POS( 20,  7 ),
    SCROLL_POS( 21,  6 ),
    SCROLL_POS( 22,  6 ),
    SCROLL_POS( 23,  5 ),
    SCROLL_POS( 24,  4 ),
    SCROLL_POS( 25,  4 ),
    SCROLL_POS( 26,  3 ),
    SCROLL_POS( 27,  3 ),
    SCROLL_POS( 28,  2 ),
    SCROLL_POS( 29,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos10[] = {
    SCROLL_POS( 34,  1 ),
    SCROLL_POS( 35,  2 ),
    SCROLL_POS( 36,  3 ),
    SCROLL_POS( 37,  3 ),
    SCROLL_POS( 38,  4 ),
    SCROLL_POS( 39,  4 ),
    SCROLL_POS( 40,  5 ),
    SCROLL_POS( 41,  6 ),
    SCROLL_POS( 42,  6 ),
    SCROLL_POS( 43,  7 ),
    SCROLL_POS( 44,  7 ),
    SCROLL_POS( 45,  7 ),
    SCROLL_POS( 46,  8 ),
    SCROLL_POS( 47,  8 ),
    SCROLL_POS( 48,  9 ),
    SCROLL_POS( 49,  9 ),
    SCROLL_POS( 50,  9 ),
    SCROLL_POS( 51,  9 ),
    SCROLL_POS( 52,  9 ),
    -1,
};
static constexpr const int16_t _scrollpos11[] = {
    SCROLL_POS( 14, 10 ),
    SCROLL_POS( 15,  9 ),
    SCROLL_POS( 16,  9 ),
    SCROLL_POS( 17,  8 ),
    SCROLL_POS( 18,  8 ),
    SCROLL_POS( 19,  7 ),
    SCROLL_POS( 20,  7 ),
    SCROLL_POS( 21,  6 ),
    SCROLL_POS( 22,  6 ),
    SCROLL_POS( 23,  5 ),
    SCROLL_POS( 24,  5 ),
    SCROLL_POS( 25,  4 ),
    SCROLL_POS( 26,  4 ),
    SCROLL_POS( 27,  3 ),
    SCROLL_POS( 28,  3 ),
    SCROLL_POS( 29,  2 ),
    SCROLL_POS( 30,  2 ),
    SCROLL_POS( 31,  1 ),
    SCROLL_POS( 32,  1 ),
    SCROLL_POS( 33,  0 ),
    SCROLL_POS( 34,  0 ),
    -1,
};
static constexpr const int16_t _scrollpos12[] = {
    SCROLL_POS( 33,  1 ),
    SCROLL_POS( 34,  2 ),
    SCROLL_POS( 35,  2 ),
    SCROLL_POS( 36,  3 ),
    SCROLL_POS( 37,  3 ),
    SCROLL_POS( 38,  4 ),
    SCROLL_POS( 39,  4 ),
    SCROLL_POS( 40,  5 ),
    SCROLL_POS( 41,  5 ),
    SCROLL_POS( 42,  6 ),
    SCROLL_POS( 43,  6 ),
    SCROLL_POS( 44,  7 ),
    SCROLL_POS( 45,  7 ),
    SCROLL_POS( 46,  8 ),
    SCROLL_POS( 47,  8 ),
    SCROLL_POS( 48,  9 ),
    SCROLL_POS( 49,  9 ),
    SCROLL_POS( 50, 10 ),
    SCROLL_POS( 51, 10 ),
    SCROLL_POS( 52, 11 ),
    SCROLL_POS( 53, 11 ),
    -1,
};
static constexpr const int16_t _scrollpos13[] = {
    SCROLL_POS( 12, 11 ),
    SCROLL_POS( 13, 10 ),
    SCROLL_POS( 14, 10 ),
    SCROLL_POS( 15,  9 ),
    SCROLL_POS( 16,  9 ),
    SCROLL_POS( 17,  8 ),
    SCROLL_POS( 18,  8 ),
    SCROLL_POS( 19,  7 ),
    SCROLL_POS( 20,  7 ),
    SCROLL_POS( 21,  6 ),
    SCROLL_POS( 22,  6 ),
    SCROLL_POS( 23,  5 ),
    SCROLL_POS( 24,  5 ),
    SCROLL_POS( 25,  4 ),
    SCROLL_POS( 26,  4 ),
    SCROLL_POS( 27,  3 ),
    SCROLL_POS( 28,  3 ),
    SCROLL_POS( 29,  2 ),
    SCROLL_POS( 30,  2 ),
    SCROLL_POS( 31,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos14[] = {
    SCROLL_POS( 33,  1 ),
    SCROLL_POS( 34,  2 ),
    SCROLL_POS( 35,  2 ),
    SCROLL_POS( 36,  3 ),
    SCROLL_POS( 37,  3 ),
    SCROLL_POS( 38,  4 ),
    SCROLL_POS( 39,  4 ),
    SCROLL_POS( 40,  5 ),
    SCROLL_POS( 41,  5 ),
    SCROLL_POS( 42,  6 ),
    SCROLL_POS( 43,  6 ),
    SCROLL_POS( 44,  7 ),
    SCROLL_POS( 45,  7 ),
    SCROLL_POS( 46,  8 ),
    SCROLL_POS( 47,  8 ),
    SCROLL_POS( 48,  9 ),
    SCROLL_POS( 49,  9 ),
    SCROLL_POS( 50, 10 ),
    SCROLL_POS( 51, 10 ),
    SCROLL_POS( 52, 11 ),
    SCROLL_POS( 53, 11 ),
    -1,
};
static constexpr const int16_t _scrollpos15[] = {
    SCROLL_POS( 10, 10 ),
    SCROLL_POS( 11, 10 ),
    SCROLL_POS( 12,  9 ),
    SCROLL_POS( 13,  9 ),
    SCROLL_POS( 14,  8 ),
    SCROLL_POS( 15,  8 ),
    SCROLL_POS( 16,  7 ),
    SCROLL_POS( 17,  7 ),
    SCROLL_POS( 18,  6 ),
    SCROLL_POS( 19,  6 ),
    SCROLL_POS( 20,  5 ),
    SCROLL_POS( 21,  5 ),
    SCROLL_POS( 22,  4 ),
    SCROLL_POS( 23,  4 ),
    SCROLL_POS( 24,  3 ),
    SCROLL_POS( 25,  3 ),
    SCROLL_POS( 26,  2 ),
    SCROLL_POS( 27,  2 ),
    SCROLL_POS( 28,  1 ),
    SCROLL_POS( 29,  1 ),
    SCROLL_POS( 30,  0 ),
    SCROLL_POS( 31,  0 ),
    -1,
};
static constexpr const int16_t _scrollpos16[] = {
    SCROLL_POS( 33,  0 ),
    SCROLL_POS( 34,  0 ),
    SCROLL_POS( 35,  1 ),
    SCROLL_POS( 36,  1 ),
    SCROLL_POS( 37,  2 ),
    SCROLL_POS( 38,  2 ),
    SCROLL_POS( 39,  3 ),
    SCROLL_POS( 40,  3 ),
    SCROLL_POS( 41,  4 ),
    SCROLL_POS( 42,  4 ),
    SCROLL_POS( 43,  5 ),
    SCROLL_POS( 44,  5 ),
    SCROLL_POS( 45,  6 ),
    SCROLL_POS( 46,  6 ),
    SCROLL_POS( 47,  7 ),
    SCROLL_POS( 48,  7 ),
    SCROLL_POS( 49,  8 ),
    SCROLL_POS( 50,  8 ),
    SCROLL_POS( 51,  9 ),
    SCROLL_POS( 52,  9 ),
    SCROLL_POS( 53, 10 ),
    SCROLL_POS( 54, 10 ),
    -1,
};
static constexpr const int16_t _scrollpos17[] = {
    SCROLL_POS(  6, 11 ),
    SCROLL_POS(  7, 11 ),
    SCROLL_POS(  8, 10 ),
    SCROLL_POS(  9, 10 ),
    SCROLL_POS( 10,  9 ),
    SCROLL_POS( 11,  9 ),
    SCROLL_POS( 12,  8 ),
    SCROLL_POS( 13,  8 ),
    SCROLL_POS( 14,  7 ),
    SCROLL_POS( 15,  7 ),
    SCROLL_POS( 16,  6 ),
    SCROLL_POS( 17,  6 ),
    SCROLL_POS( 18,  5 ),
    SCROLL_POS( 19,  5 ),
    SCROLL_POS( 20,  4 ),
    SCROLL_POS( 21,  4 ),
    SCROLL_POS( 22,  3 ),
    SCROLL_POS( 23,  3 ),
    SCROLL_POS( 24,  2 ),
    SCROLL_POS( 25,  2 ),
    SCROLL_POS( 26,  1 ),
    SCROLL_POS( 27,  1 ),
    SCROLL_POS( 28,  0 ),
    SCROLL_POS( 29,  0 ),
    -1,
};
static constexpr const int16_t _scrollpos18[] = {
    SCROLL_POS( 34,  0 ),
    SCROLL_POS( 35,  0 ),
    SCROLL_POS( 36,  1 ),
    SCROLL_POS( 37,  1 ),
    SCROLL_POS( 38,  2 ),
    SCROLL_POS( 39,  2 ),
    SCROLL_POS( 40,  3 ),
    SCROLL_POS( 41,  3 ),
    SCROLL_POS( 42,  4 ),
    SCROLL_POS( 43,  4 ),
    SCROLL_POS( 44,  5 ),
    SCROLL_POS( 45,  5 ),
    SCROLL_POS( 46,  6 ),
    SCROLL_POS( 47,  6 ),
    SCROLL_POS( 48,  7 ),
    SCROLL_POS( 49,  7 ),
    SCROLL_POS( 50,  8 ),
    SCROLL_POS( 51,  8 ),
    SCROLL_POS( 52,  9 ),
    SCROLL_POS( 53,  9 ),
    SCROLL_POS( 54, 10 ),
    SCROLL_POS( 55, 10 ),
    SCROLL_POS( 56, 11 ),
    SCROLL_POS( 57, 11 ),
    -1,
};
static constexpr const int16_t _scrollpos19[] = {
    SCROLL_POS( 13,  1 ),
    SCROLL_POS( 14,  1 ),
    SCROLL_POS( 15,  2 ),
    SCROLL_POS( 16,  2 ),
    SCROLL_POS( 17,  3 ),
    SCROLL_POS( 18,  3 ),
    SCROLL_POS( 19,  4 ),
    SCROLL_POS( 20,  4 ),
    SCROLL_POS( 21,  5 ),
    SCROLL_POS( 22,  5 ),
    SCROLL_POS( 23,  6 ),
    SCROLL_POS( 24,  6 ),
    SCROLL_POS( 25,  7 ),
    SCROLL_POS( 26,  7 ),
    SCROLL_POS( 27,  8 ),
    SCROLL_POS( 28,  8 ),
    SCROLL_POS( 29,  9 ),
    SCROLL_POS( 30,  9 ),
    SCROLL_POS( 31, 10 ),
    SCROLL_POS( 32, 10 ),
    SCROLL_POS( 33, 10 ),
    SCROLL_POS( 34,  9 ),
    SCROLL_POS( 35,  9 ),
    SCROLL_POS( 36,  8 ),
    SCROLL_POS( 37,  8 ),
    SCROLL_POS( 38,  7 ),
    SCROLL_POS( 39,  7 ),
    SCROLL_POS( 40,  6 ),
    SCROLL_POS( 41,  6 ),
    SCROLL_POS( 42,  5 ),
    SCROLL_POS( 43,  5 ),
    SCROLL_POS( 44,  4 ),
    SCROLL_POS( 45,  4 ),
    SCROLL_POS( 46,  3 ),
    SCROLL_POS( 47,  3 ),
    SCROLL_POS( 48,  2 ),
    SCROLL_POS( 49,  2 ),
    SCROLL_POS( 50,  1 ),
    SCROLL_POS( 51,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos20[] = {
    SCROLL_POS( 12,  1 ),
    SCROLL_POS( 13,  3 ),
    SCROLL_POS( 14,  4 ),
    SCROLL_POS( 15,  5 ),
    SCROLL_POS( 16,  6 ),
    SCROLL_POS( 17,  7 ),
    SCROLL_POS( 18,  7 ),
    SCROLL_POS( 19,  8 ),
    SCROLL_POS( 20,  8 ),
    SCROLL_POS( 21,  9 ),
    SCROLL_POS( 22,  9 ),
    SCROLL_POS( 23,  9 ),
    SCROLL_POS( 24, 10 ),
    SCROLL_POS( 25, 10 ),
    SCROLL_POS( 26, 10 ),
    SCROLL_POS( 27, 10 ),
    SCROLL_POS( 28, 10 ),
    SCROLL_POS( 29, 10 ),
    SCROLL_POS( 30, 10 ),
    SCROLL_POS( 31, 10 ),
    SCROLL_POS( 32, 10 ),
    SCROLL_POS( 33, 10 ),
    SCROLL_POS( 34, 10 ),
    SCROLL_POS( 35, 10 ),
    SCROLL_POS( 36, 10 ),
    SCROLL_POS( 37, 10 ),
    SCROLL_POS( 38, 10 ),
    SCROLL_POS( 39,  9 ),
    SCROLL_POS( 40,  9 ),
    SCROLL_POS( 41,  9 ),
    SCROLL_POS( 42,  8 ),
    SCROLL_POS( 43,  8 ),
    SCROLL_POS( 44,  7 ),
    SCROLL_POS( 45,  7 ),
    SCROLL_POS( 46,  6 ),
    SCROLL_POS( 47,  5 ),
    SCROLL_POS( 48,  4 ),
    SCROLL_POS( 49,  3 ),
    -1,
};
static constexpr const int16_t _scrollpos21[] = {
    SCROLL_POS( 12,  1 ),
    SCROLL_POS( 13,  1 ),
    SCROLL_POS( 14,  2 ),
    SCROLL_POS( 15,  2 ),
    SCROLL_POS( 16,  3 ),
    SCROLL_POS( 17,  3 ),
    SCROLL_POS( 18,  4 ),
    SCROLL_POS( 19,  4 ),
    SCROLL_POS( 20,  5 ),
    SCROLL_POS( 21,  5 ),
    SCROLL_POS( 22,  6 ),
    SCROLL_POS( 23,  6 ),
    SCROLL_POS( 24,  7 ),
    SCROLL_POS( 25,  7 ),
    SCROLL_POS( 26,  8 ),
    SCROLL_POS( 27,  8 ),
    SCROLL_POS( 28,  9 ),
    SCROLL_POS( 29,  9 ),
    SCROLL_POS( 30, 10 ),
    SCROLL_POS( 31, 10 ),
    SCROLL_POS( 32, 10 ),
    SCROLL_POS( 33,  9 ),
    SCROLL_POS( 34,  9 ),
    SCROLL_POS( 35,  8 ),
    SCROLL_POS( 36,  8 ),
    SCROLL_POS( 37,  7 ),
    SCROLL_POS( 38,  7 ),
    SCROLL_POS( 39,  6 ),
    SCROLL_POS( 40,  6 ),
    SCROLL_POS( 41,  5 ),
    SCROLL_POS( 42,  5 ),
    SCROLL_POS( 43,  4 ),
    SCROLL_POS( 44,  4 ),
    SCROLL_POS( 45,  3 ),
    SCROLL_POS( 46,  3 ),
    SCROLL_POS( 47,  2 ),
    SCROLL_POS( 48,  2 ),
    SCROLL_POS( 49,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos22[] = {
    SCROLL_POS( 16,  1 ),
    SCROLL_POS( 17,  1 ),
    SCROLL_POS( 18,  2 ),
    SCROLL_POS( 19,  2 ),
    SCROLL_POS( 20,  3 ),
    SCROLL_POS( 21,  3 ),
    SCROLL_POS( 22,  4 ),
    SCROLL_POS( 23,  4 ),
    SCROLL_POS( 24,  5 ),
    SCROLL_POS( 25,  5 ),
    SCROLL_POS( 26,  6 ),
    SCROLL_POS( 27,  6 ),
    SCROLL_POS( 28,  6 ),
    SCROLL_POS( 29,  6 ),
    SCROLL_POS( 30,  6 ),
    SCROLL_POS( 31,  6 ),
    SCROLL_POS( 32,  6 ),
    SCROLL_POS( 33,  6 ),
    SCROLL_POS( 34,  6 ),
    SCROLL_POS( 35,  6 ),
    SCROLL_POS( 36,  6 ),
    SCROLL_POS( 37,  6 ),
    SCROLL_POS( 38,  6 ),
    SCROLL_POS( 39,  5 ),
    SCROLL_POS( 40,  5 ),
    SCROLL_POS( 41,  4 ),
    SCROLL_POS( 42,  4 ),
    SCROLL_POS( 43,  3 ),
    SCROLL_POS( 44,  3 ),
    SCROLL_POS( 45,  2 ),
    SCROLL_POS( 46,  2 ),
    SCROLL_POS( 47,  1 ),
    SCROLL_POS( 48,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos23[] = {
    SCROLL_POS( 15,  1 ),
    SCROLL_POS( 16,  2 ),
    SCROLL_POS( 17,  2 ),
    SCROLL_POS( 18,  3 ),
    SCROLL_POS( 19,  4 ),
    SCROLL_POS( 20,  5 ),
    SCROLL_POS( 21,  5 ),
    SCROLL_POS( 22,  5 ),
    SCROLL_POS( 23,  6 ),
    SCROLL_POS( 24,  6 ),
    SCROLL_POS( 25,  6 ),
    SCROLL_POS( 26,  6 ),
    SCROLL_POS( 27,  7 ),
    SCROLL_POS( 28,  7 ),
    SCROLL_POS( 29,  7 ),
    SCROLL_POS( 30,  7 ),
    SCROLL_POS( 31,  7 ),
    SCROLL_POS( 32,  7 ),
    SCROLL_POS( 33,  7 ),
    SCROLL_POS( 34,  7 ),
    SCROLL_POS( 35,  7 ),
    SCROLL_POS( 36,  7 ),
    SCROLL_POS( 37,  6 ),
    SCROLL_POS( 38,  6 ),
    SCROLL_POS( 39,  6 ),
    SCROLL_POS( 40,  6 ),
    SCROLL_POS( 41,  5 ),
    SCROLL_POS( 42,  5 ),
    SCROLL_POS( 43,  5 ),
    SCROLL_POS( 44,  4 ),
    SCROLL_POS( 45,  3 ),
    SCROLL_POS( 46,  3 ),
    SCROLL_POS( 47,  2 ),
    SCROLL_POS( 48,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos24[] = {
    SCROLL_POS(  8,  9 ),
    SCROLL_POS(  9,  9 ),
    SCROLL_POS( 10,  8 ),
    SCROLL_POS( 11,  8 ),
    SCROLL_POS( 12,  7 ),
    SCROLL_POS( 13,  7 ),
    SCROLL_POS( 14,  6 ),
    SCROLL_POS( 15,  6 ),
    SCROLL_POS( 16,  5 ),
    SCROLL_POS( 17,  5 ),
    SCROLL_POS( 18,  4 ),
    SCROLL_POS( 19,  4 ),
    SCROLL_POS( 20,  3 ),
    SCROLL_POS( 21,  3 ),
    SCROLL_POS( 22,  2 ),
    SCROLL_POS( 23,  2 ),
    SCROLL_POS( 24,  1 ),
    SCROLL_POS( 25,  1 ),
    SCROLL_POS( 26,  0 ),
    SCROLL_POS( 27,  0 ),
    -1,
};
static constexpr const int16_t _scrollpos25[] = {
    SCROLL_POS( 36,  0 ),
    SCROLL_POS( 37,  0 ),
    SCROLL_POS( 38,  1 ),
    SCROLL_POS( 39,  1 ),
    SCROLL_POS( 40,  2 ),
    SCROLL_POS( 41,  2 ),
    SCROLL_POS( 42,  3 ),
    SCROLL_POS( 43,  3 ),
    SCROLL_POS( 44,  4 ),
    SCROLL_POS( 45,  4 ),
    SCROLL_POS( 46,  5 ),
    SCROLL_POS( 47,  5 ),
    SCROLL_POS( 48,  6 ),
    SCROLL_POS( 49,  6 ),
    SCROLL_POS( 50,  7 ),
    SCROLL_POS( 51,  7 ),
    SCROLL_POS( 52,  8 ),
    SCROLL_POS( 53,  8 ),
    SCROLL_POS( 54,  9 ),
    SCROLL_POS( 55,  9 ),
    -1,
};
static constexpr const int16_t _scrollpos26[] = {
    SCROLL_POS(  4, 13 ),
    SCROLL_POS(  5, 13 ),
    SCROLL_POS(  6, 12 ),
    SCROLL_POS(  7, 12 ),
    SCROLL_POS(  8, 11 ),
    SCROLL_POS(  9, 11 ),
    SCROLL_POS( 10, 10 ),
    SCROLL_POS( 11, 10 ),
    SCROLL_POS( 12,  9 ),
    SCROLL_POS( 13,  9 ),
    SCROLL_POS( 14,  8 ),
    SCROLL_POS( 15,  8 ),
    SCROLL_POS( 16,  7 ),
    SCROLL_POS( 17,  7 ),
    SCROLL_POS( 18,  6 ),
    SCROLL_POS( 19,  6 ),
    SCROLL_POS( 20,  5 ),
    SCROLL_POS( 21,  5 ),
    SCROLL_POS( 22,  4 ),
    SCROLL_POS( 23,  4 ),
    SCROLL_POS( 24,  3 ),
    SCROLL_POS( 25,  3 ),
    SCROLL_POS( 26,  2 ),
    SCROLL_POS( 27,  2 ),
    SCROLL_POS( 28,  1 ),
    SCROLL_POS( 29,  1 ),
    SCROLL_POS( 30,  0 ),
    SCROLL_POS( 31,  0 ),
    -1,
};
static constexpr const int16_t _scrollpos27[] = {
    SCROLL_POS( 32,  0 ),
    SCROLL_POS( 33,  0 ),
    SCROLL_POS( 34,  1 ),
    SCROLL_POS( 35,  1 ),
    SCROLL_POS( 36,  2 ),
    SCROLL_POS( 37,  2 ),
    SCROLL_POS( 38,  3 ),
    SCROLL_POS( 39,  3 ),
    SCROLL_POS( 40,  4 ),
    SCROLL_POS( 41,  4 ),
    SCROLL_POS( 42,  5 ),
    SCROLL_POS( 43,  5 ),
    SCROLL_POS( 44,  6 ),
    SCROLL_POS( 45,  6 ),
    SCROLL_POS( 46,  7 ),
    SCROLL_POS( 47,  7 ),
    SCROLL_POS( 48,  8 ),
    SCROLL_POS( 49,  8 ),
    SCROLL_POS( 50,  9 ),
    SCROLL_POS( 51,  9 ),
    SCROLL_POS( 52, 10 ),
    SCROLL_POS( 53, 10 ),
    SCROLL_POS( 54, 11 ),
    SCROLL_POS( 55, 11 ),
    SCROLL_POS( 56, 12 ),
    SCROLL_POS( 57, 12 ),
    SCROLL_POS( 58, 13 ),
    SCROLL_POS( 59, 13 ),
    -1,
};
static constexpr const int16_t _scrollpos28[] = {
    SCROLL_POS(  6, 13 ),
    SCROLL_POS(  7, 13 ),
    SCROLL_POS(  8, 12 ),
    SCROLL_POS(  9, 12 ),
    SCROLL_POS( 10, 11 ),
    SCROLL_POS( 11, 11 ),
    SCROLL_POS( 12, 10 ),
    SCROLL_POS( 13, 10 ),
    SCROLL_POS( 14,  9 ),
    SCROLL_POS( 15,  9 ),
    SCROLL_POS( 16,  8 ),
    SCROLL_POS( 17,  8 ),
    SCROLL_POS( 18,  7 ),
    SCROLL_POS( 19,  7 ),
    SCROLL_POS( 20,  6 ),
    SCROLL_POS( 21,  6 ),
    SCROLL_POS( 22,  5 ),
    SCROLL_POS( 23,  5 ),
    SCROLL_POS( 24,  4 ),
    SCROLL_POS( 25,  4 ),
    SCROLL_POS( 26,  3 ),
    SCROLL_POS( 27,  3 ),
    SCROLL_POS( 28,  2 ),
    SCROLL_POS( 29,  2 ),
    SCROLL_POS( 30,  1 ),
    SCROLL_POS( 31,  1 ),
    SCROLL_POS( 32,  0 ),
    SCROLL_POS( 33,  0 ),
    -1,
};
static constexpr const int16_t _scrollpos29[] = {
    SCROLL_POS( 30,  0 ),
    SCROLL_POS( 31,  0 ),
    SCROLL_POS( 32,  1 ),
    SCROLL_POS( 33,  1 ),
    SCROLL_POS( 34,  2 ),
    SCROLL_POS( 35,  2 ),
    SCROLL_POS( 36,  3 ),
    SCROLL_POS( 37,  3 ),
    SCROLL_POS( 38,  4 ),
    SCROLL_POS( 39,  4 ),
    SCROLL_POS( 40,  5 ),
    SCROLL_POS( 41,  5 ),
    SCROLL_POS( 42,  6 ),
    SCROLL_POS( 43,  6 ),
    SCROLL_POS( 44,  7 ),
    SCROLL_POS( 45,  7 ),
    SCROLL_POS( 46,  8 ),
    SCROLL_POS( 47,  8 ),
    SCROLL_POS( 48,  9 ),
    SCROLL_POS( 49,  9 ),
    SCROLL_POS( 50, 10 ),
    SCROLL_POS( 51, 10 ),
    SCROLL_POS( 52, 11 ),
    SCROLL_POS( 53, 11 ),
    SCROLL_POS( 54, 12 ),
    SCROLL_POS( 55, 12 ),
    SCROLL_POS( 56, 13 ),
    SCROLL_POS( 57, 13 ),
    -1,
};
static constexpr const int16_t _scrollpos30[] = {
    SCROLL_POS(  2, 30 ),
    SCROLL_POS(  3, 30 ),
    SCROLL_POS(  4, 29 ),
    SCROLL_POS(  5, 29 ),
    SCROLL_POS(  6, 28 ),
    SCROLL_POS(  7, 28 ),
    SCROLL_POS(  8, 27 ),
    SCROLL_POS(  9, 27 ),
    SCROLL_POS( 10, 26 ),
    SCROLL_POS( 11, 26 ),
    SCROLL_POS( 12, 25 ),
    SCROLL_POS( 13, 25 ),
    SCROLL_POS( 14, 24 ),
    SCROLL_POS( 15, 24 ),
    SCROLL_POS( 16, 23 ),
    SCROLL_POS( 17, 23 ),
    SCROLL_POS( 18, 22 ),
    SCROLL_POS( 19, 22 ),
    SCROLL_POS( 20, 21 ),
    SCROLL_POS( 21, 21 ),
    SCROLL_POS( 22, 20 ),
    SCROLL_POS( 23, 20 ),
    SCROLL_POS( 24, 19 ),
    SCROLL_POS( 25, 19 ),
    SCROLL_POS( 26, 18 ),
    SCROLL_POS( 27, 18 ),
    SCROLL_POS( 28, 17 ),
    SCROLL_POS( 29, 17 ),
    SCROLL_POS( 30, 16 ),
    SCROLL_POS( 31, 16 ),
    SCROLL_POS( 32, 15 ),
    SCROLL_POS( 33, 15 ),
    SCROLL_POS( 34, 14 ),
    SCROLL_POS( 35, 14 ),
    SCROLL_POS( 36, 13 ),
    SCROLL_POS( 37, 13 ),
    SCROLL_POS( 38, 12 ),
    SCROLL_POS( 39, 12 ),
    SCROLL_POS( 40, 11 ),
    SCROLL_POS( 41, 11 ),
    SCROLL_POS( 42, 10 ),
    SCROLL_POS( 43, 10 ),
    SCROLL_POS( 44,  9 ),
    SCROLL_POS( 45,  9 ),
    SCROLL_POS( 46,  8 ),
    SCROLL_POS( 47,  8 ),
    SCROLL_POS( 48,  7 ),
    SCROLL_POS( 49,  7 ),
    SCROLL_POS( 50,  6 ),
    SCROLL_POS( 51,  6 ),
    SCROLL_POS( 52,  5 ),
    SCROLL_POS( 53,  5 ),
    SCROLL_POS( 54,  4 ),
    SCROLL_POS( 55,  4 ),
    SCROLL_POS( 56,  3 ),
    SCROLL_POS( 57,  3 ),
    SCROLL_POS( 58,  2 ),
    SCROLL_POS( 59,  2 ),
    SCROLL_POS( 60,  1 ),
    SCROLL_POS( 61,  1 ),
    SCROLL_POS( 62,  0 ),
    -1,
};
static constexpr const int16_t _scrollpos31[] = {
    SCROLL_POS(  1,  0 ),
    SCROLL_POS(  2,  1 ),
    SCROLL_POS(  3,  1 ),
    SCROLL_POS(  4,  2 ),
    SCROLL_POS(  5,  2 ),
    SCROLL_POS(  6,  3 ),
    SCROLL_POS(  7,  3 ),
    SCROLL_POS(  8,  4 ),
    SCROLL_POS(  9,  4 ),
    SCROLL_POS( 10,  5 ),
    SCROLL_POS( 11,  5 ),
    SCROLL_POS( 12,  6 ),
    SCROLL_POS( 13,  6 ),
    SCROLL_POS( 14,  7 ),
    SCROLL_POS( 15,  7 ),
    SCROLL_POS( 16,  8 ),
    SCROLL_POS( 17,  8 ),
    SCROLL_POS( 18,  9 ),
    SCROLL_POS( 19,  9 ),
    SCROLL_POS( 20, 10 ),
    SCROLL_POS( 21, 10 ),
    SCROLL_POS( 22, 11 ),
    SCROLL_POS( 23, 11 ),
    SCROLL_POS( 24, 12 ),
    SCROLL_POS( 25, 12 ),
    SCROLL_POS( 26, 13 ),
    SCROLL_POS( 27, 13 ),
    SCROLL_POS( 28, 14 ),
    SCROLL_POS( 29, 14 ),
    SCROLL_POS( 30, 15 ),
    SCROLL_POS( 31, 15 ),
    SCROLL_POS( 32, 16 ),
    SCROLL_POS( 33, 16 ),
    SCROLL_POS( 34, 17 ),
    SCROLL_POS( 35, 17 ),
    SCROLL_POS( 36, 18 ),
    SCROLL_POS( 37, 18 ),
    SCROLL_POS( 38, 19 ),
    SCROLL_POS( 39, 19 ),
    SCROLL_POS( 40, 20 ),
    SCROLL_POS( 41, 20 ),
    SCROLL_POS( 42, 21 ),
    SCROLL_POS( 43, 21 ),
    SCROLL_POS( 44, 22 ),
    SCROLL_POS( 45, 22 ),
    SCROLL_POS( 46, 23 ),
    SCROLL_POS( 47, 23 ),
    SCROLL_POS( 48, 24 ),
    SCROLL_POS( 49, 24 ),
    SCROLL_POS( 50, 25 ),
    SCROLL_POS( 51, 25 ),
    SCROLL_POS( 52, 26 ),
    SCROLL_POS( 53, 26 ),
    SCROLL_POS( 54, 27 ),
    SCROLL_POS( 55, 27 ),
    SCROLL_POS( 56, 28 ),
    SCROLL_POS( 57, 28 ),
    SCROLL_POS( 58, 29 ),
    SCROLL_POS( 59, 29 ),
    SCROLL_POS( 60, 30 ),
    SCROLL_POS( 61, 30 ),
    -1,
};
static constexpr const int16_t _scrollpos32[] = {
    SCROLL_POS( 12,  0 ),
    SCROLL_POS( 13,  1 ),
    SCROLL_POS( 14,  1 ),
    SCROLL_POS( 15,  2 ),
    SCROLL_POS( 16,  2 ),
    SCROLL_POS( 17,  3 ),
    SCROLL_POS( 18,  3 ),
    SCROLL_POS( 19,  4 ),
    SCROLL_POS( 20,  4 ),
    SCROLL_POS( 21,  5 ),
    SCROLL_POS( 22,  5 ),
    SCROLL_POS( 23,  6 ),
    SCROLL_POS( 24,  6 ),
    SCROLL_POS( 25,  7 ),
    SCROLL_POS( 26,  7 ),
    SCROLL_POS( 27,  8 ),
    SCROLL_POS( 28,  8 ),
    SCROLL_POS( 29,  9 ),
    SCROLL_POS( 30,  9 ),
    SCROLL_POS( 31, 10 ),
    SCROLL_POS( 32, 10 ),
    SCROLL_POS( 33, 11 ),
    SCROLL_POS( 34, 11 ),
    SCROLL_POS( 35, 12 ),
    SCROLL_POS( 36, 12 ),
    SCROLL_POS( 37, 13 ),
    SCROLL_POS( 38, 13 ),
    SCROLL_POS( 39, 14 ),
    SCROLL_POS( 40, 14 ),
    SCROLL_POS( 41, 15 ),
    SCROLL_POS( 42, 15 ),
    SCROLL_POS( 43, 16 ),
    SCROLL_POS( 44, 16 ),
    SCROLL_POS( 45, 17 ),
    SCROLL_POS( 46, 17 ),
    SCROLL_POS( 47, 18 ),
    SCROLL_POS( 48, 18 ),
    SCROLL_POS( 49, 19 ),
    SCROLL_POS( 50, 19 ),
    -1,
};
static constexpr const int16_t _scrollpos33[] = {
    SCROLL_POS( 12, 20 ),
    SCROLL_POS( 13, 20 ),
    SCROLL_POS( 14, 19 ),
    SCROLL_POS( 15, 19 ),
    SCROLL_POS( 16, 18 ),
    SCROLL_POS( 17, 18 ),
    SCROLL_POS( 18, 17 ),
    SCROLL_POS( 19, 17 ),
    SCROLL_POS( 20, 16 ),
    SCROLL_POS( 21, 16 ),
    SCROLL_POS( 22, 15 ),
    SCROLL_POS( 23, 15 ),
    SCROLL_POS( 24, 14 ),
    SCROLL_POS( 25, 14 ),
    SCROLL_POS( 26, 13 ),
    SCROLL_POS( 27, 13 ),
    SCROLL_POS( 28, 12 ),
    SCROLL_POS( 29, 12 ),
    SCROLL_POS( 30, 11 ),
    SCROLL_POS( 31, 11 ),
    SCROLL_POS( 32, 10 ),
    SCROLL_POS( 33, 10 ),
    SCROLL_POS( 34,  9 ),
    SCROLL_POS( 35,  9 ),
    SCROLL_POS( 36,  8 ),
    SCROLL_POS( 37,  8 ),
    SCROLL_POS( 38,  7 ),
    SCROLL_POS( 39,  7 ),
    SCROLL_POS( 40,  6 ),
    SCROLL_POS( 41,  6 ),
    SCROLL_POS( 42,  5 ),
    SCROLL_POS( 43,  5 ),
    SCROLL_POS( 44,  4 ),
    SCROLL_POS( 45,  4 ),
    SCROLL_POS( 46,  3 ),
    SCROLL_POS( 47,  3 ),
    SCROLL_POS( 48,  2 ),
    SCROLL_POS( 49,  2 ),
    SCROLL_POS( 50,  1 ),
    SCROLL_POS( 51,  1 ),
    -1,
};
static constexpr const int16_t _scrollpos34[] = {
    SCROLL_POS(  2, 14 ),
    SCROLL_POS(  3, 14 ),
    SCROLL_POS(  4, 13 ),
    SCROLL_POS(  5, 13 ),
    SCROLL_POS(  6, 12 ),
    SCROLL_POS(  7, 12 ),
    SCROLL_POS(  8, 11 ),
    SCROLL_POS(  9, 11 ),
    SCROLL_POS( 10, 10 ),
    SCROLL_POS( 11, 10 ),
    SCROLL_POS( 12,  9 ),
    SCROLL_POS( 13,  9 ),
    SCROLL_POS( 14,  8 ),
    SCROLL_POS( 15,  8 ),
    SCROLL_POS( 16,  7 ),
    SCROLL_POS( 17,  7 ),
    SCROLL_POS( 18,  6 ),
    SCROLL_POS( 19,  6 ),
    SCROLL_POS( 20,  5 ),
    SCROLL_POS( 21,  5 ),
    SCROLL_POS( 22,  4 ),
    SCROLL_POS( 23,  4 ),
    SCROLL_POS( 24,  3 ),
    SCROLL_POS( 25,  3 ),
    SCROLL_POS( 26,  2 ),
    SCROLL_POS( 27,  2 ),
    SCROLL_POS( 28,  1 ),
    SCROLL_POS( 29,  1 ),
    SCROLL_POS( 30,  0 ),
    -1,
};
static constexpr const int16_t _scrollpos35[] = {
    SCROLL_POS( 33,  0 ),
    SCROLL_POS( 34,  0 ),
    SCROLL_POS( 35,  1 ),
    SCROLL_POS( 36,  1 ),
    SCROLL_POS( 37,  2 ),
    SCROLL_POS( 38,  2 ),
    SCROLL_POS( 39,  3 ),
    SCROLL_POS( 40,  3 ),
    SCROLL_POS( 41,  4 ),
    SCROLL_POS( 42,  4 ),
    SCROLL_POS( 43,  5 ),
    SCROLL_POS( 44,  5 ),
    SCROLL_POS( 45,  6 ),
    SCROLL_POS( 46,  6 ),
    SCROLL_POS( 47,  7 ),
    SCROLL_POS( 48,  7 ),
    SCROLL_POS( 49,  8 ),
    SCROLL_POS( 50,  8 ),
    SCROLL_POS( 51,  9 ),
    SCROLL_POS( 52,  9 ),
    SCROLL_POS( 53, 10 ),
    SCROLL_POS( 54, 10 ),
    SCROLL_POS( 55, 11 ),
    SCROLL_POS( 56, 11 ),
    SCROLL_POS( 57, 12 ),
    SCROLL_POS( 58, 12 ),
    SCROLL_POS( 59, 13 ),
    SCROLL_POS( 60, 13 ),
    SCROLL_POS( 61, 14 ),
    -1,
};
static constexpr const int16_t _scrollpos36[] = {
    SCROLL_POS(  4,  0 ),
    SCROLL_POS(  5,  1 ),
    SCROLL_POS(  6,  2 ),
    SCROLL_POS(  7,  3 ),
    SCROLL_POS(  8,  3 ),
    SCROLL_POS(  9,  4 ),
    SCROLL_POS( 10,  5 ),
    SCROLL_POS( 11,  5 ),
    SCROLL_POS( 12,  6 ),
    SCROLL_POS( 13,  6 ),
    SCROLL_POS( 14,  7 ),
    SCROLL_POS( 15,  7 ),
    SCROLL_POS( 16,  8 ),
    SCROLL_POS( 17,  8 ),
    SCROLL_POS( 18,  9 ),
    SCROLL_POS( 19,  9 ),
    SCROLL_POS( 20, 10 ),
    SCROLL_POS( 21, 10 ),
    SCROLL_POS( 22, 10 ),
    SCROLL_POS( 23, 11 ),
    SCROLL_POS( 24, 11 ),
    SCROLL_POS( 25, 11 ),
    SCROLL_POS( 26, 12 ),
    SCROLL_POS( 27, 12 ),
    SCROLL_POS( 28, 12 ),
    SCROLL_POS( 29, 12 ),
    SCROLL_POS( 30, 12 ),
    -1,
};
static constexpr const int16_t _scrollpos37[] = {
    SCROLL_POS( 32, 13 ),
    SCROLL_POS( 33, 12 ),
    SCROLL_POS( 34, 12 ),
    SCROLL_POS( 35, 12 ),
    SCROLL_POS( 36, 12 ),
    SCROLL_POS( 37, 11 ),
    SCROLL_POS( 38, 11 ),
    SCROLL_POS( 39, 11 ),
    SCROLL_POS( 40, 10 ),
    SCROLL_POS( 41, 10 ),
    SCROLL_POS( 42, 10 ),
    SCROLL_POS( 43,  9 ),
    SCROLL_POS( 44,  9 ),
    SCROLL_POS( 45,  8 ),
    SCROLL_POS( 46,  8 ),
    SCROLL_POS( 47,  7 ),
    SCROLL_POS( 48,  7 ),
    SCROLL_POS( 49,  6 ),
    SCROLL_POS( 50,  6 ),
    SCROLL_POS( 51,  5 ),
    SCROLL_POS( 52,  5 ),
    SCROLL_POS( 53,  4 ),
    SCROLL_POS( 54,  3 ),
    SCROLL_POS( 55,  3 ),
    SCROLL_POS( 56,  2 ),
    SCROLL_POS( 57,  1 ),
    SCROLL_POS( 58,  0 ),
    -1,
};

static constexpr const int16_t* _scrollPositions[MAX_SCROLLING_TEXT_MODES] = {
    _scrollpos0,
    _scrollpos1,
    _scrollpos2,
    _scrollpos3,
    _scrollpos4,
    _scrollpos5,
    _scrollpos6,
    _scrollpos7,
    _scrollpos8,
    _scrollpos9,
    _scrollpos10,
    _scrollpos11,
    _scrollpos12,
    _scrollpos13,
    _scrollpos14,
    _scrollpos15,
    _scrollpos16,
    _scrollpos17,
    _scrollpos18,
    _scrollpos19,
    _scrollpos20,
    _scrollpos21,
    _scrollpos22,
    _scrollpos23,
    _scrollpos24,
    _scrollpos25,
    _scrollpos26,
    _scrollpos27,
    _scrollpos28,
    _scrollpos29,
    _scrollpos30,
    _scrollpos31,
    _scrollpos32,
    _scrollpos33,
    _scrollpos34,
    _scrollpos35,
    _scrollpos36,
    _scrollpos37,
};
// clang-format on

void scrolling_text_invalidate()
{
    for (auto& scrollText : _drawScrollTextList)
    {
        scrollText.string_id = 0;
        std::memset(scrollText.string_args, 0, sizeof(scrollText.string_args));
    }
}

int32_t scrolling_text_setup(
    paint_session& session, rct_string_id stringId, Formatter& ft, uint16_t scroll, uint16_t scrollingMode, colour_t colour)
{
    std::scoped_lock<std::mutex> lock(_scrollingTextMutex);

    assert(scrollingMode < MAX_SCROLLING_TEXT_MODES);

    rct_drawpixelinfo* dpi = &session.DPI;

    if (dpi->zoom_level > ZoomLevel{ 0 })
        return SPR_SCROLLING_TEXT_DEFAULT;

    _drawSCrollNextIndex++;
    ft.Rewind();
    int32_t scrollIndex = scrolling_text_get_matching_or_oldest(stringId, ft, scroll, scrollingMode, colour);
    if (scrollIndex >= SPR_SCROLLING_TEXT_START)
        return scrollIndex;

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
    scrolling_text_format(scrollString, 256, scrollText);

    const int16_t* scrollingModePositions = _scrollPositions[scrollingMode];

    std::fill_n(scrollText->bitmap, 320 * 8, 0x00);
    auto context = OpenRCT2::GetContext();
    if (LocalisationService_UseTrueTypeFont(context))
    {
        scrolling_text_set_bitmap_for_ttf(scrollString, scroll, scrollText->bitmap, scrollingModePositions, colour);
    }
    else
    {
        scrolling_text_set_bitmap_for_sprite(scrollString, scroll, scrollText->bitmap, scrollingModePositions, colour);
    }

    uint32_t imageId = SPR_SCROLLING_TEXT_START + scrollIndex;
    drawing_engine_invalidate_image(imageId);
    return imageId;
}

static void scrolling_text_set_bitmap_for_sprite(
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
                    auto characterWidth = font_sprite_get_codepoint_width(FontSpriteBase::TINY, codepoint);
                    auto characterBitmap = font_sprite_get_codepoint_bitmap(codepoint);
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
                auto g1 = gfx_get_g1_element(SPR_TEXT_PALETTE);
                if (g1 != nullptr)
                {
                    auto colourIndex = FormatTokenGetTextColourIndex(token.kind);
                    characterColour = g1->offset[colourIndex * 4];
                }
            }
        }
    }
}

static void scrolling_text_set_bitmap_for_ttf(
    std::string_view text, int32_t scroll, uint8_t* bitmap, const int16_t* scrollPositionOffsets, colour_t colour)
{
#ifndef NO_TTF
    auto fontDesc = ttf_get_font_from_sprite_base(FontSpriteBase::TINY);
    if (fontDesc->font == nullptr)
    {
        scrolling_text_set_bitmap_for_sprite(text, scroll, bitmap, scrollPositionOffsets, colour);
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
            auto g1 = gfx_get_g1_element(SPR_TEXT_PALETTE);
            if (g1 != nullptr)
            {
                auto colourIndex = FormatTokenGetTextColourIndex(token.kind);
                colour = g1->offset[colourIndex * 4];
            }
        }
    }

    auto surface = ttf_surface_cache_get_or_add(fontDesc->font, ttfBuffer.c_str());
    if (surface == nullptr)
    {
        return;
    }

    int32_t pitch = surface->pitch;
    int32_t width = surface->w;
    auto src = static_cast<const uint8_t*>(surface->pixels);

    // Pitch offset
    src += 2 * pitch;

    // Line height offset
    int32_t min_vpos = -fontDesc->offset_y;
    int32_t max_vpos = std::min(surface->h - 2, min_vpos + 7);

    bool use_hinting = gConfigFonts.enable_hinting && fontDesc->hinting_threshold > 0;

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
                    uint8_t src_pixel = src[y * pitch + x];
                    if ((!use_hinting && src_pixel != 0) || src_pixel > 140)
                    {
                        // Centre of the glyph: use full colour.
                        *dst = colour;
                    }
                    else if (use_hinting && src_pixel > fontDesc->hinting_threshold)
                    {
                        // Simulate font hinting by shading the background colour instead.
                        *dst = blendColours(colour, *dst);
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
