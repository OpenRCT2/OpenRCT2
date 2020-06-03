/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4127) // conditional expression is constant

#include "Drawing.h"

#include <cstring>

template<DrawBlendOp TBlendOp> static bool FASTCALL BlitPixel(const uint8_t* src, uint8_t* dst, const PaletteMap& paletteMap)
{
    if constexpr (TBlendOp & BLEND_TRANSPARENT)
    {
        // Ignore transparent pixels
        if (*src == 0)
        {
            return false;
        }
    }

    if constexpr (((TBlendOp & BLEND_SRC) != 0) && ((TBlendOp & BLEND_DST) != 0))
    {
        auto pixel = paletteMap.Blend(*src, *dst);
        if constexpr (TBlendOp & BLEND_TRANSPARENT)
        {
            if (pixel == 0)
            {
                return false;
            }
        }
        *dst = pixel;
        return true;
    }
    else if constexpr ((TBlendOp & BLEND_SRC) != 0)
    {
        auto pixel = paletteMap[*src];
        if constexpr (TBlendOp & BLEND_TRANSPARENT)
        {
            if (pixel == 0)
            {
                return false;
            }
        }
        *dst = pixel;
        return true;
    }
    else if constexpr ((TBlendOp & BLEND_DST) != 0)
    {
        auto pixel = paletteMap[*dst];
        if constexpr (TBlendOp & BLEND_TRANSPARENT)
        {
            if (pixel == 0)
            {
                return false;
            }
        }
        *dst = pixel;
        return true;
    }
    else
    {
        *dst = *src;
        return true;
    }
}

template<DrawBlendOp TBlendOp>
static void FASTCALL BlitPixels(const uint8_t* src, uint8_t* dst, const PaletteMap& paletteMap, uint8_t zoom, size_t dstPitch)
{
    auto yDstSkip = dstPitch - zoom;
    for (uint8_t yy = 0; yy < zoom; yy++)
    {
        for (uint8_t xx = 0; xx < zoom; xx++)
        {
            BlitPixel<TBlendOp>(src, dst, paletteMap);
            dst++;
        }
        dst += yDstSkip;
    }
}

template<DrawBlendOp TBlendOp, size_t TZoom> static void FASTCALL DrawRLESpriteMagnify(DrawSpriteArgs& args)
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
}

template<DrawBlendOp TBlendOp, int32_t zoom_level> static void FASTCALL DrawRLESpriteMinify(DrawSpriteArgs& args)
{
    auto dpi = args.DPI;
    auto source_bits_pointer = args.SourceImage.offset;
    auto dest_bits_pointer = args.DestinationBits;
    auto source_x_start = args.SrcX;
    auto source_y_start = args.SrcY;
    auto width = args.Width;
    auto height = args.Height;
    [[maybe_unused]] auto& paletteMap = args.PalMap;

    // The distance between two samples in the source image.
    // We draw the image at 1 / (2^zoom_level) scale.
    int32_t zoom_amount = 1 << zoom_level;

    // Width of one screen line in the dest buffer
    int32_t line_width = (dpi->width >> zoom_level) + dpi->pitch;

    // Move up to the first line of the image if source_y_start is negative. Why does this even occur?
    if (source_y_start < 0)
    {
        source_y_start += zoom_amount;
        height -= zoom_amount;
        dest_bits_pointer += line_width;
    }

    // For every line in the image
    for (int32_t i = 0; i < height; i += zoom_amount)
    {
        int32_t y = source_y_start + i;

        // The first part of the source pointer is a list of offsets to different lines
        // This will move the pointer to the correct source line.
        const uint16_t lineOffset = source_bits_pointer[y * 2] | (source_bits_pointer[y * 2 + 1] << 8);
        const uint8_t* lineData = source_bits_pointer + lineOffset;
        uint8_t* loop_dest_pointer = dest_bits_pointer + line_width * (i >> zoom_level);

        uint8_t isEndOfLine = 0;

        // For every data chunk in the line
        while (!isEndOfLine)
        {
            const uint8_t* copySrc = lineData;

            // Read chunk metadata
            uint8_t dataSize = *copySrc++;
            uint8_t firstPixelX = *copySrc++;

            isEndOfLine = dataSize & 0x80; // If the last bit in dataSize is set, then this is the last line
            dataSize &= 0x7F;              // The rest of the bits are the actual size

            // Have our next source pointer point to the next data section
            lineData = copySrc + dataSize;

            int32_t x_start = firstPixelX - source_x_start;
            int32_t numPixels = dataSize;

            if (x_start > 0)
            {
                int mod = x_start & (zoom_amount - 1); // x_start modulo zoom_amount

                // If x_start is not a multiple of zoom_amount, round it up to a multiple
                if (mod != 0)
                {
                    int offset = zoom_amount - mod;
                    x_start += offset;
                    copySrc += offset;
                    numPixels -= offset;
                }
            }
            else if (x_start < 0)
            {
                // Clamp x_start to zero if negative
                int offset = 0 - x_start;
                x_start = 0;
                copySrc += offset;
                numPixels -= offset;
            }

            // If the end position is further out than the whole image
            // end position then we need to shorten the line again
            if (x_start + numPixels > width)
                numPixels = width - x_start;

            uint8_t* copyDest = loop_dest_pointer + (x_start >> zoom_level);

            // Finally after all those checks, copy the image onto the drawing surface
            // If the image type is not a basic one we require to mix the pixels
            if constexpr ((TBlendOp & BLEND_SRC) != 0) // palette controlled images
            {
                for (int j = 0; j < numPixels; j += zoom_amount, copySrc += zoom_amount, copyDest++)
                {
                    if ((TBlendOp & BLEND_DST) != 0)
                    {
                        *copyDest = paletteMap.Blend(*copySrc, *copyDest);
                    }
                    else
                    {
                        *copyDest = paletteMap[*copySrc];
                    }
                }
            }
            else if constexpr ((TBlendOp & BLEND_DST) != 0) // single alpha blended color (used for glass)
            {
                for (int j = 0; j < numPixels; j += zoom_amount, copyDest++)
                {
                    *copyDest = paletteMap[*copyDest];
                }
            }
            else // standard opaque image
            {
                if (zoom_level == 0)
                {
                    // Since we're sampling each pixel at this zoom level, just do a straight std::memcpy
                    if (numPixels > 0)
                        std::memcpy(copyDest, copySrc, numPixels);
                }
                else
                {
                    for (int j = 0; j < numPixels; j += zoom_amount, copySrc += zoom_amount, copyDest++)
                        *copyDest = *copySrc;
                }
            }
        }
    }
}

template<DrawBlendOp TBlendOp> static void FASTCALL DrawRLESprite(DrawSpriteArgs& args)
{
    auto zoom_level = static_cast<int8_t>(args.DPI->zoom_level);
    switch (zoom_level)
    {
        case -2:
            DrawRLESpriteMagnify<TBlendOp, 2>(args);
            break;
        case -1:
            DrawRLESpriteMagnify<TBlendOp, 1>(args);
            break;
        case 0:
            DrawRLESpriteMinify<TBlendOp, 0>(args);
            break;
        case 1:
            DrawRLESpriteMinify<TBlendOp, 1>(args);
            break;
        case 2:
            DrawRLESpriteMinify<TBlendOp, 2>(args);
            break;
        case 3:
            DrawRLESpriteMinify<TBlendOp, 3>(args);
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
