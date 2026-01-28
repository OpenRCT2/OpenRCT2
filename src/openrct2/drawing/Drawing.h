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
#include "../core/StringTypes.h"
#include "../interface/ZoomLevel.h"
#include "../world/Location.hpp"
#include "ColourPalette.h"
#include "FilterPaletteIds.h"
#include "Font.h"
#include "G1Element.h"
#include "ImageId.hpp"
#include "RenderTarget.h"
#include "Text.h"
#include "TextColour.h"

#include <array>
#include <cassert>
#include <memory>
#include <optional>
#include <span>
#include <vector>

struct ScreenCoordsXY;
struct ScreenLine;
struct ScreenRect;

namespace OpenRCT2
{
    struct ColourWithFlags;
    struct IPlatformEnvironment;
    struct IStream;
} // namespace OpenRCT2

namespace OpenRCT2::Drawing
{
    struct IDrawingEngine;
    enum class FilterPaletteID : int32_t;
} // namespace OpenRCT2::Drawing

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

namespace OpenRCT2::Drawing
{
    struct TranslucentWindowPalette
    {
        FilterPaletteID base;
        FilterPaletteID highlight;
        FilterPaletteID shadow;
    };
} // namespace OpenRCT2::Drawing

/**
 * Represents an 8-bit indexed map that maps from one palette index to another.
 */
struct PaletteMap
{
private:
    std::span<OpenRCT2::Drawing::PaletteIndex> _data{};
#ifdef _DEBUG
    // We only require those fields for the asserts in debug builds.
    size_t _numMaps{};
    size_t _mapLength{};
#endif

public:
    static PaletteMap GetDefault();

    constexpr PaletteMap() = default;

    constexpr PaletteMap(OpenRCT2::Drawing::PaletteIndex* data, size_t numMaps, size_t mapLength)
        : _data{ data, numMaps * mapLength }
#ifdef _DEBUG
        , _numMaps(numMaps)
        , _mapLength(mapLength)
#endif
    {
    }

    constexpr PaletteMap(std::span<OpenRCT2::Drawing::PaletteIndex> map)
        : _data(map)
#ifdef _DEBUG
        , _numMaps(1)
        , _mapLength(map.size())
#endif
    {
    }

    OpenRCT2::Drawing::PaletteIndex& operator[](size_t index);
    OpenRCT2::Drawing::PaletteIndex operator[](size_t index) const;

    OpenRCT2::Drawing::PaletteIndex Blend(OpenRCT2::Drawing::PaletteIndex src, OpenRCT2::Drawing::PaletteIndex dst) const;
    void Copy(
        OpenRCT2::Drawing::PaletteIndex dstIndex, const PaletteMap& src, OpenRCT2::Drawing::PaletteIndex srcIndex,
        size_t length);
};

struct DrawSpriteArgs
{
    ImageId Image;
    PaletteMap PalMap;
    const OpenRCT2::G1Element& SourceImage;
    int32_t SrcX;
    int32_t SrcY;
    int32_t Width;
    int32_t Height;
    uint8_t* DestinationBits;

    DrawSpriteArgs(
        ImageId image, const PaletteMap& palMap, const OpenRCT2::G1Element& sourceImage, int32_t srcX, int32_t srcY,
        int32_t width, int32_t height, uint8_t* destinationBits)
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
bool FASTCALL
    BlitPixel(const OpenRCT2::Drawing::PaletteIndex* src, OpenRCT2::Drawing::PaletteIndex* dst, const PaletteMap& paletteMap)
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
void FASTCALL BlitPixels(const uint8_t* src, uint8_t* dst, const PaletteMap& paletteMap, uint8_t zoom, size_t dstPitch)
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

constexpr uint8_t kPaletteTotalOffsets = 192;

extern OpenRCT2::Drawing::GamePalette gPalette;
extern OpenRCT2::Drawing::GamePalette gGamePalette;
extern uint32_t gPaletteEffectFrame;

extern OpenRCT2::Drawing::TextColours gTextPalette;
extern const OpenRCT2::Drawing::TranslucentWindowPalette kTranslucentWindowPalettes[OpenRCT2::Drawing::kColourNumTotal];

extern ImageId gPickupPeepImage;
extern int32_t gPickupPeepX;
extern int32_t gPickupPeepY;
extern bool gPaintForceRedraw;

bool ClipRenderTarget(
    OpenRCT2::Drawing::RenderTarget& dst, OpenRCT2::Drawing::RenderTarget& src, const ScreenCoordsXY& coords, int32_t width,
    int32_t height);
void GfxSetDirtyBlocks(const ScreenRect& rect);
void GfxInvalidateScreen();

// palette
void GfxTransposePalette(ImageIndex pal, uint8_t product);
void LoadPalette();

// other
void GfxClear(OpenRCT2::Drawing::RenderTarget& rt, OpenRCT2::Drawing::PaletteIndex paletteIndex);
void GfxFilterPixel(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, OpenRCT2::Drawing::FilterPaletteID palette);
void GfxInvalidatePickedUpPeep();
void GfxDrawPickedUpPeep(OpenRCT2::Drawing::RenderTarget& rt);

// line
void GfxDrawLine(OpenRCT2::Drawing::RenderTarget& rt, const ScreenLine& line, OpenRCT2::Drawing::PaletteIndex colour);
void GfxDrawLineSoftware(OpenRCT2::Drawing::RenderTarget& rt, const ScreenLine& line, OpenRCT2::Drawing::PaletteIndex colour);
void GfxDrawDashedLine(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenLine& screenLine, int32_t dashedLineSegmentLength,
    OpenRCT2::Drawing::PaletteIndex colour);

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
void FASTCALL GfxDrawSprite(OpenRCT2::Drawing::RenderTarget& rt, ImageId image_id, const ScreenCoordsXY& coords);
void FASTCALL GfxDrawGlyph(
    OpenRCT2::Drawing::RenderTarget& rt, ImageId image, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL GfxDrawSpriteSolid(
    OpenRCT2::Drawing::RenderTarget& rt, ImageId image, const ScreenCoordsXY& coords, OpenRCT2::Drawing::PaletteIndex colour);
void FASTCALL GfxDrawSpriteRawMasked(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, ImageId maskImage, ImageId colourImage);
void FASTCALL GfxDrawSpriteSoftware(OpenRCT2::Drawing::RenderTarget& rt, ImageId imageId, const ScreenCoordsXY& spriteCoords);
void FASTCALL GfxDrawSpritePaletteSetSoftware(
    OpenRCT2::Drawing::RenderTarget& rt, ImageId imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL GfxDrawSpriteRawMaskedSoftware(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& scrCoords, ImageId maskImage, ImageId colourImage);

// string
void GfxDrawStringLeftCentred(
    OpenRCT2::Drawing::RenderTarget& rt, StringId format, void* args, ColourWithFlags colour, const ScreenCoordsXY& coords);
void DrawStringCentredRaw(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t numLines, const utf8* text, FontStyle fontStyle);
void DrawNewsTicker(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, OpenRCT2::Drawing::Colour colour,
    StringId format, u8string_view args, int32_t ticks);
void GfxDrawStringWithYOffsets(
    OpenRCT2::Drawing::RenderTarget& rt, const utf8* text, ColourWithFlags colour, const ScreenCoordsXY& coords,
    const int8_t* yOffsets, bool forceSpriteFont, FontStyle fontStyle);

int32_t GfxWrapString(u8string_view text, int32_t width, FontStyle fontStyle, u8string* outWrappedText, int32_t* outNumLines);
int32_t GfxGetStringWidth(std::string_view text, FontStyle fontStyle);
int32_t GfxGetStringWidthNewLined(std::string_view text, FontStyle fontStyle);
int32_t GfxGetStringWidthNoFormatting(std::string_view text, FontStyle fontStyle);
int32_t StringGetHeightRaw(std::string_view text, FontStyle fontStyle);
int32_t GfxClipString(char* buffer, int32_t width, FontStyle fontStyle);
u8string ShortenPath(const u8string& path, int32_t availableWidth, FontStyle fontStyle);
void TTFDrawString(
    OpenRCT2::Drawing::RenderTarget& rt, const_utf8string text, ColourWithFlags colour, const ScreenCoordsXY& coords,
    bool noFormatting, FontStyle fontStyle, TextDarkness darkness);

size_t G1CalculateDataSize(const OpenRCT2::G1Element* g1);

void MaskScalar(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
void MaskSse4_1(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
void MaskAvx2(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);

void MaskFn(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);

std::optional<uint32_t> GetPaletteG1Index(OpenRCT2::Drawing::FilterPaletteID paletteId);
std::optional<PaletteMap> GetPaletteMapForColour(OpenRCT2::Drawing::FilterPaletteID paletteId);
void UpdatePalette(
    std::span<const OpenRCT2::Drawing::BGRAColour> palette, OpenRCT2::Drawing::PaletteIndex startIndex, int32_t numColours);
void UpdatePaletteEffects();

void RefreshVideo();
void ToggleWindowedMode();

void DebugRT(OpenRCT2::Drawing::RenderTarget& rt);

#include "NewDrawing.h"
