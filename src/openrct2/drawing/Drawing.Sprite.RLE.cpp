/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

#include <cassert>
#include <cstring>

template<DrawBlendOp TBlendOp>
static void FASTCALL DrawRLESpriteMagnify(DrawPixelInfo& dpi, const DrawSpriteArgs& args)
{
    auto& paletteMap = args.PalMap;
    auto imgData = args.SourceImage.offset;
    auto dst = args.DestinationBits;
    auto srcX = args.SrcX;
    auto srcY = args.SrcY;
    auto width = args.Width;
    auto height = args.Height;
    auto zoom = dpi.zoom_level;
    auto dstLineWidth = dpi.LineStride();

    for (int32_t y = 0; y < height; y++)
    {
        uint8_t* nextDst = dst + dstLineWidth;
        const int32_t rowNum = zoom.ApplyTo(srcY + y);
        uint16_t lineOffset;
        std::memcpy(&lineOffset, &imgData[rowNum * sizeof(uint16_t)], sizeof(uint16_t));
        const uint8_t* data8 = imgData + lineOffset;

        bool lastDataForLine = false;
        int32_t numPixels = 0;
        uint8_t pixelRunStart = 0;
        for (int32_t x = 0; x < width; x++)
        {
            const int32_t colNum = zoom.ApplyTo(srcX + x);

            while (colNum >= pixelRunStart + numPixels && !lastDataForLine)
            {
                data8 += numPixels;
                numPixels = *data8++;
                pixelRunStart = *data8++;
                lastDataForLine = numPixels & 0x80;
                numPixels &= 0x7F;
            }
            if (pixelRunStart <= colNum && colNum < pixelRunStart + numPixels)
                BlitPixel<TBlendOp>(data8 + colNum - pixelRunStart, dst, paletteMap);
            dst++;
        }

        dst = nextDst;
    }
}

template<DrawBlendOp TBlendOp, size_t TZoom>
static void FASTCALL DrawRLESpriteMinify(DrawPixelInfo& dpi, const DrawSpriteArgs& args)
{
    auto src0 = args.SourceImage.offset;
    auto dst0 = args.DestinationBits;
    auto srcX = args.SrcX;
    auto srcY = args.SrcY;
    auto width = args.Width;
    auto height = args.Height;
    auto zoom = 1 << TZoom;
    auto dstLineWidth = static_cast<size_t>(dpi.LineStride());

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
            if constexpr ((TBlendOp & kBlendSrc) == 0 && (TBlendOp & kBlendDst) == 0 && TZoom == 0)
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

template<DrawBlendOp TBlendOp>
static void FASTCALL DrawRLESprite(DrawPixelInfo& dpi, const DrawSpriteArgs& args)
{
    auto zoom_level = static_cast<int8_t>(dpi.zoom_level);
    switch (zoom_level)
    {
        case -2:
        case -1:
            DrawRLESpriteMagnify<TBlendOp>(dpi, args);
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
void FASTCALL GfxRleSpriteToBuffer(DrawPixelInfo& dpi, const DrawSpriteArgs& args)
{
    if (args.Image.HasPrimary())
    {
        if (args.Image.IsBlended())
        {
            DrawRLESprite<kBlendTransparent | kBlendSrc | kBlendDst>(dpi, args);
        }
        else
        {
            DrawRLESprite<kBlendTransparent | kBlendSrc>(dpi, args);
        }
    }
    else if (args.Image.IsBlended())
    {
        DrawRLESprite<kBlendTransparent | kBlendDst>(dpi, args);
    }
    else
    {
        DrawRLESprite<kBlendTransparent>(dpi, args);
    }
}
