/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

template<DrawBlendOp TBlendOp> static void FASTCALL DrawBMPSpriteMagnify(DrawSpriteArgs& args)
{
    auto& g1 = args.SourceImage;
    auto src = g1.offset + ((static_cast<size_t>(g1.width) * args.SrcY) + args.SrcX);
    auto dst = args.DestinationBits;
    auto& paletteMap = args.PalMap;
    auto dpi = args.DPI;
    auto zoomLevel = dpi->zoom_level;
    size_t srcLineWidth = g1.width;
    size_t dstLineWidth = (static_cast<size_t>(dpi->width) / zoomLevel) + dpi->pitch;
    uint8_t zoom = 1 / zoomLevel;
    auto width = args.Width / zoomLevel;
    auto height = args.Height / zoomLevel;
    for (; height > 0; height -= zoom)
    {
        auto nextSrc = src + srcLineWidth;
        auto nextDst = dst + (dstLineWidth * zoom);
        for (int32_t widthRemaining = width; widthRemaining > 0; widthRemaining -= zoom, src++, dst += zoom)
        {
            // Copy src to a block of zoom * zoom on dst
            BlitPixels<TBlendOp>(src, dst, paletteMap, zoom, dstLineWidth);
        }
        src = nextSrc;
        dst = nextDst;
    }
}

template<DrawBlendOp TBlendOp> static void FASTCALL DrawBMPSpriteMinify(DrawSpriteArgs& args)
{
    auto& g1 = args.SourceImage;
    auto src = g1.offset + ((static_cast<size_t>(g1.width) * args.SrcY) + args.SrcX);
    auto dst = args.DestinationBits;
    auto& paletteMap = args.PalMap;
    auto width = args.Width;
    auto height = args.Height;
    auto dpi = args.DPI;
    auto zoomLevel = dpi->zoom_level;
    size_t srcLineWidth = g1.width * zoomLevel;
    size_t dstLineWidth = (static_cast<size_t>(dpi->width) / zoomLevel) + dpi->pitch;
    uint8_t zoom = 1 * zoomLevel;
    for (; height > 0; height -= zoom)
    {
        auto nextSrc = src + srcLineWidth;
        auto nextDst = dst + dstLineWidth;
        for (int32_t widthRemaining = width; widthRemaining > 0; widthRemaining -= zoom, src += zoom, dst++)
        {
            BlitPixel<TBlendOp>(src, dst, paletteMap);
        }
        src = nextSrc;
        dst = nextDst;
    }
}

template<DrawBlendOp TBlendOp> static void FASTCALL DrawBMPSprite(DrawSpriteArgs& args)
{
    if (args.DPI->zoom_level < 0)
    {
        DrawBMPSpriteMagnify<TBlendOp>(args);
    }
    else
    {
        DrawBMPSpriteMinify<TBlendOp>(args);
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
    auto imageId = args.Image;

    // Image uses the palette pointer to remap the colours of the image
    if (imageId.HasPrimary())
    {
        if (imageId.IsBlended())
        {
            // Copy non-transparent bitmap data but blend src and dst pixel using the palette map.
            DrawBMPSprite<BLEND_TRANSPARENT | BLEND_SRC | BLEND_DST>(args);
        }
        else
        {
            // Copy non-transparent bitmap data but re-colour using the palette map.
            DrawBMPSprite<BLEND_TRANSPARENT | BLEND_SRC>(args);
        }
    }
    else if (imageId.IsBlended())
    {
        // Image is only a transparency mask. Just colour the pixels using the palette map.
        // Used for glass.
        DrawBMPSprite<BLEND_TRANSPARENT | BLEND_DST>(args);
        return;
    }
    else if (!(args.SourceImage.flags & G1_FLAG_BMP))
    {
        // Copy raw bitmap data to target
        DrawBMPSprite<BLEND_NONE>(args);
    }
    else
    {
        // Copy raw bitmap data to target but exclude transparent pixels
        DrawBMPSprite<BLEND_TRANSPARENT>(args);
    }
}
