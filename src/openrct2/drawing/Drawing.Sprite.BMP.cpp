/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

template<DrawBlendOp TBlendOp>
static void FASTCALL DrawBMPSpriteMagnify(RenderTarget& rt, const DrawSpriteArgs& args)
{
    auto& paletteMap = args.PalMap;
    auto src0 = args.SourceImage.offset;
    auto dst = reinterpret_cast<PaletteIndex*>(args.DestinationBits);
    auto srcX = args.SrcX;
    auto srcY = args.SrcY;
    auto width = args.Width;
    auto height = args.Height;
    auto zoom = rt.zoom_level;
    auto dstLineWidth = rt.LineStride();
    auto srcLineWidth = args.SourceImage.width;

    for (int32_t y = 0; y < height; y++)
    {
        auto nextDst = dst + dstLineWidth;
        for (int32_t x = 0; x < width; x++, dst++)
        {
            PaletteIndex* src = reinterpret_cast<PaletteIndex*>(
                src0 + (srcLineWidth * zoom.ApplyTo(srcY + y) + zoom.ApplyTo(srcX + x)));
            BlitPixel<TBlendOp>(src, dst, paletteMap);
        }
        dst = nextDst;
    }
}

template<DrawBlendOp TBlendOp>
static void FASTCALL DrawBMPSpriteMinify(RenderTarget& rt, const DrawSpriteArgs& args)
{
    auto& g1 = args.SourceImage;
    auto* src = reinterpret_cast<PaletteIndex*>(g1.offset + ((static_cast<size_t>(g1.width) * args.SrcY) + args.SrcX));
    auto* dst = reinterpret_cast<PaletteIndex*>(args.DestinationBits);
    auto& paletteMap = args.PalMap;
    auto width = args.Width;
    auto height = args.Height;
    auto zoomLevel = rt.zoom_level;
    size_t srcLineWidth = zoomLevel.ApplyTo(g1.width);
    size_t dstLineWidth = rt.LineStride();
    uint8_t zoom = zoomLevel.ApplyTo(1);
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

template<DrawBlendOp TBlendOp>
static void FASTCALL DrawBMPSprite(RenderTarget& rt, const DrawSpriteArgs& args)
{
    if (rt.zoom_level < ZoomLevel{ 0 })
    {
        DrawBMPSpriteMagnify<TBlendOp>(rt, args);
    }
    else
    {
        DrawBMPSpriteMinify<TBlendOp>(rt, args);
    }
}

/**
 * Copies a sprite onto the buffer. There is no compression used on the sprite
 * image.
 *  rct2: 0x0067A690
 * @param imageId Only flags are used.
 */
void FASTCALL GfxBmpSpriteToBuffer(RenderTarget& rt, const DrawSpriteArgs& args)
{
    auto imageId = args.Image;

    // Image uses the palette pointer to remap the colours of the image
    if (imageId.HasPrimary())
    {
        if (imageId.IsBlended())
        {
            // Copy non-transparent bitmap data but blend src and dst pixel using the palette map.
            DrawBMPSprite<kBlendTransparent | kBlendSrc | kBlendDst>(rt, args);
        }
        else
        {
            // Copy non-transparent bitmap data but re-colour using the palette map.
            DrawBMPSprite<kBlendTransparent | kBlendSrc>(rt, args);
        }
    }
    else if (imageId.IsBlended())
    {
        // Image is only a transparency mask. Just colour the pixels using the palette map.
        // Used for glass.
        DrawBMPSprite<kBlendTransparent | kBlendDst>(rt, args);
    }
    else if (!args.SourceImage.flags.has(G1Flag::hasTransparency))
    {
        // Copy raw bitmap data to target
        DrawBMPSprite<kBlendNone>(rt, args);
    }
    else
    {
        // Copy raw bitmap data to target but exclude transparent pixels
        DrawBMPSprite<kBlendTransparent>(rt, args);
    }
}
