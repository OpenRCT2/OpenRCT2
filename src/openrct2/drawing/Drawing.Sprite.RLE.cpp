/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

#include <algorithm>
#include <cstring>

template<DrawBlendOp TBlendOp, size_t TZoom, bool with_checks = false>
static bool FASTCALL DrawRLESpriteMagnify(DrawSpriteArgs& args, [[maybe_unused]] size_t buffer_length = 0)
{
    auto dpi = args.DPI;
    auto src0 = args.SourceImage.offset;
    auto dst0 = args.DestinationBits;
    auto srcX = args.SrcX;
    auto srcY = args.SrcY;
    auto width = args.Width;
    auto height = args.Height;
    auto& paletteMap = args.PalMap;
    auto zoom = 1 << TZoom;
    auto dstLineWidth = (static_cast<size_t>(dpi->width) << TZoom) + dpi->pitch;

    // Move up to the first line of the image if source_y_start is negative. Why does this even occur?
    if (srcY < 0)
    {
        srcY += zoom;
        height -= zoom;
        dst0 += dstLineWidth;
    }

    // For every line in the image
    for (int32_t i = 0; i < height; i++)
    {
        int32_t y = srcY + i;

        // The first part of the source pointer is a list of offsets to different lines
        // This will move the pointer to the correct source line.
        uint16_t lineOffset = src0[y * 2] | (src0[y * 2 + 1] << 8);
        auto nextRun = src0 + lineOffset;
        auto dstLineStart = dst0 + ((dstLineWidth * i) << TZoom);

        // For every data chunk in the line
        bool isEndOfLine = false;
        while (!isEndOfLine)
        {
            // Read chunk metadata
            auto src = nextRun;
            auto dataSize = *src++;
            auto firstPixelX = *src++;
            isEndOfLine = (dataSize & 0x80) != 0;
            dataSize &= 0x7F;

            // Have our next source pointer point to the next data section
            nextRun = src + dataSize;

            int32_t x = firstPixelX - srcX;
            int32_t numPixels = dataSize;
            if (x < 0)
            {
                src += -x;
                numPixels += x;
                x = 0;
            }

            // If the end position is further out than the whole image
            // end position then we need to shorten the line again
            numPixels = std::min(numPixels, width - x);

            auto dst = dstLineStart + (static_cast<size_t>(x) << TZoom);
            while (numPixels > 0)
            {
                BlitPixels<TBlendOp>(src, dst, paletteMap, zoom, dstLineWidth);
                src++;
                dst += zoom;
                numPixels--;
            }
        }
    }
    return true;
}

template<DrawBlendOp TBlendOp, size_t TZoom, bool with_checks = false>
static bool FASTCALL DrawRLESpriteMinify(DrawSpriteArgs& args, [[maybe_unused]] size_t buffer_length = 0)
{
    auto dpi = args.DPI;
    auto src0 = args.SourceImage.offset;
    auto dst0 = args.DestinationBits;
    auto srcX = args.SrcX;
    auto srcY = args.SrcY;
    auto width = args.Width;
    auto height = args.Height;
    auto zoom = 1 << TZoom;
    auto dstLineWidth = (static_cast<size_t>(dpi->width) >> TZoom) + dpi->pitch;
    [[maybe_unused]] auto srcEnd = src0 + buffer_length;

    // Move up to the first line of the image if source_y_start is negative. Why does this even occur?
    if (srcY < 0)
    {
        srcY += zoom;
        height -= zoom;
        dst0 += dstLineWidth;
    }

    // For every line in the image
    for (int32_t i = 0; i < height; i += zoom)
    {
        int32_t y = srcY + i;

        if constexpr (with_checks)
        {
            const auto ptr = &src0[y * 2 + 1];
            if (ptr >= srcEnd)
            {
                return false;
            }
        }

        // The first part of the source pointer is a list of offsets to different lines
        // This will move the pointer to the correct source line.
        uint16_t lineOffset = src0[y * 2] | (src0[y * 2 + 1] << 8);
        auto nextRun = src0 + lineOffset;
        auto dstLineStart = dst0 + dstLineWidth * (i >> TZoom);

        // For every data chunk in the line
        auto isEndOfLine = false;
        while (!isEndOfLine)
        {
            // Read chunk metadata
            auto src = nextRun;

            if constexpr (with_checks)
            {
                const auto ptr = src + 2;
                if (ptr > srcEnd)
                {
                    return false;
                }
            }

            auto dataSize = *src++;
            auto firstPixelX = *src++;
            isEndOfLine = (dataSize & 0x80) != 0;
            dataSize &= 0x7F;

            // Have our next source pointer point to the next data section
            nextRun = src + dataSize;

            int32_t x = firstPixelX - srcX;
            int32_t numPixels = dataSize;
            if (x > 0)
            {
                // If x is not a multiple of zoom, round it up to a multiple
                auto mod = x & (zoom - 1);
                if (mod != 0)
                {
                    auto offset = zoom - mod;
                    x += offset;
                    src += offset;
                    numPixels -= offset;
                }
            }
            else if (x < 0)
            {
                // Clamp x to zero if negative
                src += -x;
                numPixels += x;
                x = 0;
            }

            // If the end position is further out than the whole image
            // end position then we need to shorten the line again
            numPixels = std::min(numPixels, width - x);

            auto dst = dstLineStart + (x >> TZoom);
            if constexpr (with_checks)
            {
                const auto ptr = src + numPixels;
                if (numPixels > 0 && ptr > srcEnd)
                {
                    return false;
                }
            }
            else
            {
                if constexpr ((TBlendOp & BLEND_SRC) == 0 && (TBlendOp & BLEND_DST) == 0 && TZoom == 0)
                {
                    // Since we're sampling each pixel at this zoom level, just do a straight std::memcpy
                    if (numPixels > 0)
                    {
                        std::memcpy(dst, src, numPixels);
                    }
                }
                else
                {
                    auto& paletteMap = args.PalMap;
                    while (numPixels > 0)
                    {
                        BlitPixel<TBlendOp>(src, dst, paletteMap);
                        numPixels -= zoom;
                        src += zoom;
                        dst++;
                    }
                }
            }
        }
    }
    return true;
}

template<DrawBlendOp TBlendOp, bool with_checks = false>
static bool FASTCALL DrawRLESprite(DrawSpriteArgs& args, [[maybe_unused]] size_t buffer_length = 0)
{
    auto zoom_level = static_cast<int8_t>(args.DPI->zoom_level);
    switch (zoom_level)
    {
        case -2:
            return DrawRLESpriteMagnify<TBlendOp, 2, with_checks>(args, buffer_length);
        case -1:
            return DrawRLESpriteMagnify<TBlendOp, 1, with_checks>(args, buffer_length);
        case 0:
            return DrawRLESpriteMinify<TBlendOp, 0, with_checks>(args, buffer_length);
        case 1:
            return DrawRLESpriteMinify<TBlendOp, 1, with_checks>(args, buffer_length);
        case 2:
            return DrawRLESpriteMinify<TBlendOp, 2, with_checks>(args, buffer_length);
        case 3:
            return DrawRLESpriteMinify<TBlendOp, 3, with_checks>(args, buffer_length);
        default:
            assert(false);
            return false;
    }
}

/**
 * Transfers readied images onto buffers
 * This function copies the sprite data onto the screen
 *  rct2: 0x0067AA18
 * @param imageId Only flags are used.
 */
void FASTCALL gfx_rle_sprite_to_buffer(DrawSpriteArgs& args)
{
    if (args.Image.HasPrimary())
    {
        if (args.Image.IsBlended())
        {
            DrawRLESprite<BLEND_TRANSPARENT | BLEND_SRC | BLEND_DST>(args);
        }
        else
        {
            DrawRLESprite<BLEND_TRANSPARENT | BLEND_SRC>(args);
        }
    }
    else if (args.Image.IsBlended())
    {
        DrawRLESprite<BLEND_TRANSPARENT | BLEND_DST>(args);
    }
    else
    {
        DrawRLESprite<BLEND_TRANSPARENT>(args);
    }
}

/**
 * Verifies the inbound sprite for out-of-bounds accesses. Does not copy any data.
 */
bool FASTCALL check_gfx_rle_sprite_to_buffer(DrawSpriteArgs& args, size_t buffer_length)
{
    if (args.Image.HasPrimary())
    {
        if (args.Image.IsBlended())
        {
            return DrawRLESprite<BLEND_TRANSPARENT | BLEND_SRC | BLEND_DST, true>(args, buffer_length);
        }
        else
        {
            return DrawRLESprite<BLEND_TRANSPARENT | BLEND_SRC, true>(args, buffer_length);
        }
    }
    else if (args.Image.IsBlended())
    {
        return DrawRLESprite<BLEND_TRANSPARENT | BLEND_DST, true>(args, buffer_length);
    }
    else
    {
        return DrawRLESprite<BLEND_TRANSPARENT, true>(args, buffer_length);
    }
}
