/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

static void FASTCALL gfx_bmp_sprite_to_buffer_magnify(DrawSpriteArgs& args)
{
    auto dpi = args.DPI;
    auto zoom_level = dpi->zoom_level;
    uint8_t zoom_amount = 1 * zoom_level;
    uint32_t dest_line_width = (dpi->width / zoom_level) + dpi->pitch;
    uint32_t source_line_width = args.SourceImage.width * zoom_level;

    // Basic bitmap with no draw pixels
    auto source_pointer = args.SourceBits;
    auto dest_pointer = args.DestinationBits;
    auto width = args.Width;
    auto height = args.Height;
    for (; height > 0; height -= zoom_amount)
    {
        auto next_source_pointer = source_pointer + source_line_width;
        auto next_dest_pointer = dest_pointer + dest_line_width;

        for (int32_t no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++, source_pointer += zoom_amount)
        {
            uint8_t pixel = *source_pointer;
            if (pixel)
            {
                *dest_pointer = pixel;
            }
        }
        dest_pointer = next_dest_pointer;
        source_pointer = next_source_pointer;
    }
}

template<DrawBlendOp TBlendOp> static void FASTCALL gfx_bmp_sprite_to_buffer_x(DrawSpriteArgs& args)
{
    auto src = args.SourceBits;
    auto dst = args.DestinationBits;
    auto& paletteMap = args.PalMap;
    auto width = args.Width;
    auto height = args.Height;
    auto dpi = args.DPI;
    auto zoomLevel = dpi->zoom_level;
    size_t srcLineWidth = args.SourceImage.width * zoomLevel;
    size_t dstLineWidth = (static_cast<size_t>(dpi->width) / zoomLevel) + dpi->pitch;
    uint8_t zoom = 1 * zoomLevel;
    for (; height > 0; height -= zoom)
    {
        auto nextSrc = src + srcLineWidth;
        auto nextDst = dst + dstLineWidth;
        for (int32_t widthRemaining = width; widthRemaining > 0; widthRemaining -= zoom, src += zoom, dst++)
        {
            if constexpr (TBlendOp & BLEND_TRANSPARENT)
            {
                // Ignore transparent pixels
                if (*src == 0)
                {
                    continue;
                }
            }

            if constexpr (((TBlendOp & BLEND_SRC) != 0) && ((TBlendOp & BLEND_DST) != 0))
            {
                *dst = paletteMap.Blend(*src, *dst);
            }
            else if constexpr ((TBlendOp & BLEND_SRC) != 0)
            {
                *dst = paletteMap[*src];
            }
            else if constexpr ((TBlendOp & BLEND_DST) != 0)
            {
                *dst = paletteMap[*dst];
            }
            else
            {
                *dst = *src;
            }
        }
        src = nextSrc;
        dst = nextDst;
    }
}

/**
 * Copies a sprite onto the buffer. There is no compression used on the sprite
 * image.
 *  rct2: 0x0067A690
 * @param imageId Only flags are used.
 */
void FASTCALL gfx_bmp_sprite_to_buffer(DrawSpriteArgs& args)
{
    auto dpi = args.DPI;
    auto zoom_level = dpi->zoom_level;
    if (zoom_level < 0)
    {
        gfx_bmp_sprite_to_buffer_magnify(args);
        return;
    }

    auto imageId = args.Image;

    // Image uses the palette pointer to remap the colours of the image
    if (imageId.HasPrimary())
    {
        // Copy non-transparent bitmap data but re-colour using the palette map.
        gfx_bmp_sprite_to_buffer_x<BLEND_TRANSPARENT | BLEND_SRC>(args);
    }
    else if (imageId.IsBlended())
    {
        // Image is only a transparency mask. Just colour the pixels using the palette map.
        // Used for glass.
        gfx_bmp_sprite_to_buffer_x<BLEND_TRANSPARENT | BLEND_DST>(args);
        return;
    }
    else if (!(args.SourceImage.flags & G1_FLAG_BMP))
    {
        // Copy raw bitmap data to target
        gfx_bmp_sprite_to_buffer_x<BLEND_NONE>(args);
    }
    else
    {
        // Copy raw bitmap data to target but exclude transparent pixels
        gfx_bmp_sprite_to_buffer_x<BLEND_TRANSPARENT>(args);
    }
}
