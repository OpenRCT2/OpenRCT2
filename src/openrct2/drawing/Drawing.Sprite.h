/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/CallingConventions.h"
#include "G1Element.h"
#include "ImageId.hpp"
#include "PaletteIndex.h"
#include "PaletteMap.h"
#include "RenderTarget.h"

#include <cstdint>
#include <optional>
#include <vector>

struct ScreenCoordsXY;

namespace OpenRCT2
{
    struct IPlatformEnvironment;
} // namespace OpenRCT2

using DrawBlendOp = uint8_t;

constexpr DrawBlendOp kBlendNone = 0;

/**
 * Only supported by BITMAP. RLE images always encode transparency via the encoding.
 * Pixel value of 0 represents transparent.
 */
constexpr DrawBlendOp kBlendTransparent = 1 << 0;

/**
 * Whether to use the pixel value from the source image.
 * This is usually only unset for glass images where there the src is only a transparency mask.
 */
constexpr DrawBlendOp kBlendSrc = 1 << 1;

/**
 * Whether to use the pixel value of the destination image for blending.
 * This is used for any image that filters the target image, e.g. glass or water.
 */
constexpr DrawBlendOp kBlendDst = 2 << 2;

struct DrawSpriteArgs
{
    ImageId Image;
    OpenRCT2::Drawing::PaletteMap PalMap;
    const OpenRCT2::G1Element& SourceImage;
    int32_t SrcX;
    int32_t SrcY;
    int32_t Width;
    int32_t Height;
    OpenRCT2::Drawing::PaletteIndex* DestinationBits;

    DrawSpriteArgs(
        ImageId image, const OpenRCT2::Drawing::PaletteMap& palMap, const OpenRCT2::G1Element& sourceImage, int32_t srcX,
        int32_t srcY, int32_t width, int32_t height, OpenRCT2::Drawing::PaletteIndex* destinationBits)
        : Image(image)
        , PalMap(palMap)
        , SourceImage(sourceImage)
        , SrcX(srcX)
        , SrcY(srcY)
        , Width(width)
        , Height(height)
        , DestinationBits(destinationBits)
    {
    }
};

template<DrawBlendOp TBlendOp>
bool FASTCALL BlitPixel(
    const OpenRCT2::Drawing::PaletteIndex* src, OpenRCT2::Drawing::PaletteIndex* dst,
    const OpenRCT2::Drawing::PaletteMap& paletteMap)
{
    if constexpr (TBlendOp & kBlendTransparent)
    {
        // Ignore transparent pixels
        if (*src == OpenRCT2::Drawing::PaletteIndex::transparent)
        {
            return false;
        }
    }

    if constexpr (((TBlendOp & kBlendSrc) != 0) && ((TBlendOp & kBlendDst) != 0))
    {
        auto pixel = paletteMap.Blend(*src, *dst);
        if constexpr (TBlendOp & kBlendTransparent)
        {
            if (pixel == OpenRCT2::Drawing::PaletteIndex::transparent)
            {
                return false;
            }
        }
        *dst = pixel;
        return true;
    }
    else if constexpr ((TBlendOp & kBlendSrc) != 0)
    {
        auto pixel = paletteMap[EnumValue(*src)];
        if constexpr (TBlendOp & kBlendTransparent)
        {
            if (pixel == OpenRCT2::Drawing::PaletteIndex::transparent)
            {
                return false;
            }
        }
        *dst = pixel;
        return true;
    }
    else if constexpr ((TBlendOp & kBlendDst) != 0)
    {
        auto pixel = paletteMap[EnumValue(*dst)];
        if constexpr (TBlendOp & kBlendTransparent)
        {
            if (pixel == OpenRCT2::Drawing::PaletteIndex::transparent)
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
void FASTCALL
    BlitPixels(const uint8_t* src, uint8_t* dst, const OpenRCT2::Drawing::PaletteMap& paletteMap, uint8_t zoom, size_t dstPitch)
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

// sprite catalogues
bool GfxLoadG1(const OpenRCT2::IPlatformEnvironment& env);
void GfxLoadG2PalettesFontsTracks();
bool GfxLoadCsg();
void GfxUnloadG1();
void GfxUnloadG2PalettesFontsTracks();
void GfxUnloadCsg();
const OpenRCT2::G1Element* GfxGetG1Element(ImageId imageId);
const OpenRCT2::G1Element* GfxGetG1Element(ImageIndex image_id);
const OpenRCT2::G1Palette* GfxGetG1Palette(ImageIndex imageId);
void GfxSetG1Element(ImageIndex imageId, const OpenRCT2::G1Element* g1);
std::optional<OpenRCT2::Gx> GfxLoadGx(const std::vector<uint8_t>& buffer);
bool IsCsgLoaded();

// sprite blitting
void FASTCALL GfxSpriteToBuffer(OpenRCT2::Drawing::RenderTarget& rt, const DrawSpriteArgs& args);
void FASTCALL GfxBmpSpriteToBuffer(OpenRCT2::Drawing::RenderTarget& rt, const DrawSpriteArgs& args);
void FASTCALL GfxRleSpriteToBuffer(OpenRCT2::Drawing::RenderTarget& rt, const DrawSpriteArgs& args);
void FASTCALL GfxDrawSpriteSoftware(OpenRCT2::Drawing::RenderTarget& rt, ImageId imageId, const ScreenCoordsXY& spriteCoords);
void FASTCALL GfxDrawSpritePaletteSetSoftware(
    OpenRCT2::Drawing::RenderTarget& rt, ImageId imageId, const ScreenCoordsXY& coords,
    const OpenRCT2::Drawing::PaletteMap& paletteMap);
void FASTCALL GfxDrawSpriteRawMaskedSoftware(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& scrCoords, ImageId maskImage, ImageId colourImage);

// sprite data
size_t G1CalculateDataSize(const OpenRCT2::G1Element* g1);

void MaskScalar(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc,
    OpenRCT2::Drawing::PaletteIndex* RESTRICT dst, int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
