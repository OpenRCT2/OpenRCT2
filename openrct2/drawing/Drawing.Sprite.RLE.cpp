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

template<DrawBlendOp TBlendOp, size_t TZoom>
static void FASTCALL DrawRLESpriteMagnify(rct_drawpixelinfo& dpi, const DrawSpriteArgs& args)
{
    auto src0 = args.SourceImage.offset;
    auto dst0 = args.DestinationBits;
    auto srcX = args.SrcX;
    auto srcY = args.SrcY;
    auto width = args.Width;
    auto height = args.Height;
    auto& paletteMap = args.PalMap;
    auto zoom = 1 << TZoom;
    auto dstLineWidth = (static_cast<size_t>(dpi.width) << TZoom) + dpi.pitch;

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
}

template<DrawBlendOp TBlendOp, size_t TZoom>
static void FASTCALL DrawRLESpriteMinify(rct_drawpixelinfo& dpi, const DrawSpriteArgs& args)
{
    auto src0 = args.SourceImage.offset;
    auto dst0 = args.DestinationBits;
    auto srcX = args.SrcX;
    auto srcY = args.SrcY;
    auto width = args.Width;
    auto height = args.Height;
    auto zoom = 1 << TZoom;
    auto dstLineWidth = (static_cast<size_t>(dpi.width) >> TZoom) + dpi.pitch;

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

template<DrawBlendOp TBlendOp> static void FASTCALL DrawRLESprite(rct_drawpixelinfo& dpi, const DrawSpriteArgs& args)
{
    auto zoom_level = static_cast<int8_t>(dpi.zoom_level);
    switch (zoom_level)
    {
        case -2:
            DrawRLESpriteMagnify<TBlendOp, 2>(dpi, args);
            break;
        case -1:
            DrawRLESpriteMagnify<TBlendOp, 1>(dpi, args);
            break;
        case 0:
            DrawRLESpriteMinify<TBlendOp, 0>(dpi, args);
            break;
        case 1:
            DrawRLESpriteMinify<TBlendOp, 1>(dpi, args);
            break;
        case 2:
            DrawRLESpriteMinify<TBlendOp, 2>(dpi, args);
            break;
        case 3:
            DrawRLESpriteMinify<TBlendOp, 3>(dpi, args);
            break;
        default:
            assert(false);
            break;
    }
}

/**
 * Transfers readied images onto buffers
 * This function copies the sprite data onto the screen
 *  rct2: 0x0067AA18
 * @param imageId Only flags are used.
 */
void FASTCALL gfx_rle_sprite_to_buffer(rct_drawpixelinfo& dpi, const DrawSpriteArgs& args)
{
    if (args.Image.HasPrimary())
    {
        if (args.Image.IsBlended())
        {
            DrawRLESprite<BLEND_TRANSPARENT | BLEND_SRC | BLEND_DST>(dpi, args);
        }
        else
        {
            DrawRLESprite<BLEND_TRANSPARENT | BLEND_SRC>(dpi, args);
        }
    }
    else if (args.Image.IsBlended())
    {
        DrawRLESprite<BLEND_TRANSPARENT | BLEND_DST>(dpi, args);
    }
    else
    {
        DrawRLESprite<BLEND_TRANSPARENT>(dpi, args);
    }
}
