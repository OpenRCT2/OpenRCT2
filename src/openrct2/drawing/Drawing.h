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
#include "Drawing.Sprite.h"
#include "FilterPaletteIds.h"
#include "G1Element.h"
#include "ImageId.hpp"
#include "PaletteMap.h"
#include "RenderTarget.h"

#include <array>
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
    enum class PaletteIndex : uint8_t;
} // namespace OpenRCT2::Drawing

namespace OpenRCT2::Drawing
{
    struct TranslucentWindowPalette
    {
        FilterPaletteID base;
        FilterPaletteID highlight;
        FilterPaletteID shadow;
    };
} // namespace OpenRCT2::Drawing

constexpr uint8_t kPaletteTotalOffsets = 192;

extern OpenRCT2::Drawing::GamePalette gPalette;
extern OpenRCT2::Drawing::GamePalette gGamePalette;
extern uint32_t gPaletteEffectFrame;

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

// sprite (NewDrawing.cpp functions that use the drawing engine)
void FASTCALL GfxDrawSprite(OpenRCT2::Drawing::RenderTarget& rt, ImageId image_id, const ScreenCoordsXY& coords);
void FASTCALL GfxDrawGlyph(
    OpenRCT2::Drawing::RenderTarget& rt, ImageId image, const ScreenCoordsXY& coords,
    const OpenRCT2::Drawing::PaletteMap& paletteMap);
void FASTCALL GfxDrawSpriteSolid(
    OpenRCT2::Drawing::RenderTarget& rt, ImageId image, const ScreenCoordsXY& coords, OpenRCT2::Drawing::PaletteIndex colour);
void FASTCALL GfxDrawSpriteRawMasked(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, ImageId maskImage, ImageId colourImage);

void MaskSse4_1(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc,
    OpenRCT2::Drawing::PaletteIndex* RESTRICT dst, int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
void MaskAvx2(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc,
    OpenRCT2::Drawing::PaletteIndex* RESTRICT dst, int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);

void MaskFn(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc,
    OpenRCT2::Drawing::PaletteIndex* RESTRICT dst, int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);

std::optional<uint32_t> GetPaletteG1Index(OpenRCT2::Drawing::FilterPaletteID paletteId);
std::optional<OpenRCT2::Drawing::PaletteMap> GetPaletteMapForColour(OpenRCT2::Drawing::FilterPaletteID paletteId);
void UpdatePalette(
    std::span<const OpenRCT2::Drawing::BGRAColour> palette, OpenRCT2::Drawing::PaletteIndex startIndex, int32_t numColours);
void UpdatePaletteEffects();

void RefreshVideo();
void ToggleWindowedMode();

void DebugRT(OpenRCT2::Drawing::RenderTarget& rt);

#include "NewDrawing.h"
