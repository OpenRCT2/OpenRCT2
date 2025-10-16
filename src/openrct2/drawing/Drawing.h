/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/CallingConventions.h"
#include "../core/StringTypes.h"
#include "../interface/Colour.h"
#include "../interface/ZoomLevel.h"
#include "../world/Location.hpp"
#include "ColourPalette.h"
#include "Font.h"
#include "ImageId.hpp"
#include "Text.h"

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
    struct IPlatformEnvironment;
    struct IStream;
} // namespace OpenRCT2

namespace OpenRCT2::Drawing
{
    struct IDrawingEngine;
}

struct G1Element
{
    uint8_t* offset = nullptr; // 0x00
    int16_t width = 0;         // 0x04
    int16_t height = 0;        // 0x06
    int16_t x_offset = 0;      // 0x08
    int16_t y_offset = 0;      // 0x0A
    uint16_t flags = 0;        // 0x0C
    int32_t zoomed_offset = 0; // 0x0E
};

#pragma pack(push, 1)
struct RCTG1Header
{
    uint32_t num_entries = 0;
    uint32_t total_size = 0;
};
static_assert(sizeof(RCTG1Header) == 8);
#pragma pack(pop)

struct Gx
{
    RCTG1Header header;
    std::vector<G1Element> elements;
    std::unique_ptr<uint8_t[]> data;
};

struct RenderTarget
{
    uint8_t* bits{};
    int32_t x{};
    int32_t y{};
    int32_t width{};
    int32_t height{};
    int32_t pitch{}; // note: this is actually (pitch - width)
    int32_t cullingX{};
    int32_t cullingY{};
    int32_t cullingWidth{};
    int32_t cullingHeight{};
    ZoomLevel zoom_level{};

    // Last position of drawn text.
    ScreenCoordsXY lastStringPos{};

    OpenRCT2::Drawing::IDrawingEngine* DrawingEngine{};

    uint8_t* GetBitsOffset(const ScreenCoordsXY& pos) const;
    RenderTarget Crop(const ScreenCoordsXY& pos, const ScreenSize& size) const;

    [[nodiscard]] constexpr int32_t WorldX() const
    {
        return zoom_level.ApplyTo(x);
    }
    [[nodiscard]] constexpr int32_t WorldY() const
    {
        return zoom_level.ApplyTo(y);
    }
    [[nodiscard]] constexpr int32_t WorldWidth() const
    {
        return zoom_level.ApplyTo(width);
    }
    [[nodiscard]] constexpr int32_t WorldHeight() const
    {
        return zoom_level.ApplyTo(height);
    }

    [[nodiscard]] constexpr int32_t LineStride() const
    {
        return width + pitch;
    }
};

enum : uint32_t
{
    TEXT_DRAW_FLAG_NO_FORMATTING = 1 << 28,
    TEXT_DRAW_FLAG_Y_OFFSET_EFFECT = 1 << 29,
    TEXT_DRAW_FLAG_TTF = 1 << 30,
    TEXT_DRAW_FLAG_NO_DRAW = 1u << 31
};

struct RCTG1Element
{
    uint32_t offset;        // 0x00 note: uint32_t always!
    int16_t width;          // 0x04
    int16_t height;         // 0x06
    int16_t x_offset;       // 0x08
    int16_t y_offset;       // 0x0A
    uint16_t flags;         // 0x0C
    uint16_t zoomed_offset; // 0x0E
};
static_assert(sizeof(RCTG1Element) == 0x10);

enum
{
    G1_FLAG_HAS_TRANSPARENCY = (1 << 0), // Image data contains transparent pixels (0XFF) which will not be rendered
    G1_FLAG_1 = (1 << 1),
    G1_FLAG_RLE_COMPRESSION = (1 << 2), // Image data is encoded using RCT2's form of run length encoding
    G1_FLAG_PALETTE = (1 << 3),         // Image data is a sequence of palette entries R8G8B8
    G1_FLAG_HAS_ZOOM_SPRITE = (1 << 4), // Use a different sprite for higher zoom levels
    G1_FLAG_NO_ZOOM_DRAW = (1 << 5),    // Does not get drawn at higher zoom levels (only zoom 0)
};

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

enum class FilterPaletteID : int32_t
{
    paletteNull = 0,

    paletteWater = COLOUR_COUNT,

    paletteLandMarker0, // North (corner/edge)
    paletteLandMarker1, // East (corner/edge)
    paletteLandMarker2, // South (corner/edge)
    paletteLandMarker3, // West (corner/edge)
    paletteSceneryGroundMarker,
    paletteWaterMarker,
    paletteQuarterMarker0, // North (not sure why it couldn't just use PaletteLandMarker0)
    paletteQuarterMarker1, // East
    paletteQuarterMarker2, // South
    paletteQuarterMarker3, // West
    paletteRideGroundMarker,
    paletteGhost, // Construction marker
    palette45,    // Decolourise + lighten
    palette46,

    paletteDarken3,

    paletteDarken1 = paletteDarken3 + 2,
    paletteDarken2,
    palette51, // Decolourise + darken
    paletteTranslucentGrey,
    paletteTranslucentGreyHighlight,
    paletteTranslucentGreyShadow,
    paletteTranslucentLightBlue,
    paletteTranslucentLightBlueHighlight,
    paletteTranslucentLightBlueShadow,
    paletteTranslucentBordeauxRed,
    paletteTranslucentBordeauxRedHighlight,
    paletteTranslucentBordeauxRedShadow,
    paletteTranslucentDarkGreen,
    paletteTranslucentDarkGreenHighlight,
    paletteTranslucentDarkGreenShadow,
    paletteTranslucentLightPurple,
    paletteTranslucentLightPurpleHighlight,
    paletteTranslucentLightPurpleShadow,
    paletteTranslucentDarkOliveGreen,
    paletteTranslucentDarkOliveGreenHighlight,
    paletteTranslucentDarkOliveGreenShadow,
    paletteTranslucentLightBrown,
    paletteTranslucentLightBrownHighlight,
    paletteTranslucentLightBrownShadow,
    paletteTranslucentYellow,
    paletteTranslucentYellowHighlight,
    paletteTranslucentYellowShadow,
    paletteTranslucentMossGreen,
    paletteTranslucentMossGreenHighlight,
    paletteTranslucentMossGreenShadow,
    paletteTranslucentOliveGreen,
    paletteTranslucentOliveGreenHighlight,
    paletteTranslucentOliveGreenShadow,
    paletteTranslucentBrightGreen,
    paletteTranslucentBrightGreenHighlight,
    paletteTranslucentBrightGreenShadow,
    paletteTranslucentSalmonPink,
    paletteTranslucentSalmonPinkHighlight,
    paletteTranslucentSalmonPinkShadow,
    paletteTranslucentBrightPurple,
    paletteTranslucentBrightPurpleHighlight,
    paletteTranslucentBrightPurpleShadow,
    paletteTranslucentBrightRed,
    paletteTranslucentBrightRedHighlight,
    paletteTranslucentBrightRedShadow,
    paletteTranslucentLightOrange,
    paletteTranslucentLightOrangeHighlight,
    paletteTranslucentLightOrangeShadow,
    paletteTranslucentTeal,
    paletteTranslucentTealHighlight,
    paletteTranslucentTealShadow,
    paletteTranslucentBrightPink,
    paletteTranslucentBrightPinkHighlight,
    paletteTranslucentBrightPinkShadow,
    paletteTranslucentDarkBrown,
    paletteTranslucentDarkBrownHighlight,
    paletteTranslucentDarkBrownShadow,
    paletteTranslucentLightPink,
    paletteTranslucentLightPinkHighlight,
    paletteTranslucentLightPinkShadow,
    paletteTranslucentWhite,
    paletteTranslucentWhiteHighlight,
    paletteTranslucentWhiteShadow,
    paletteGlass,
    paletteGlassBlack = paletteGlass + COLOUR_BLACK,
    paletteGlassGrey = paletteGlass + COLOUR_GREY,
    paletteGlassWhite = paletteGlass + COLOUR_WHITE,
    paletteGlassDarkPurple = paletteGlass + COLOUR_DARK_PURPLE,
    paletteGlassLightPurple = paletteGlass + COLOUR_LIGHT_PURPLE,
    paletteGlassBrightPurple = paletteGlass + COLOUR_BRIGHT_PURPLE,
    paletteGlassDarkBlue = paletteGlass + COLOUR_DARK_BLUE,
    paletteGlassLightBlue = paletteGlass + COLOUR_LIGHT_BLUE,
    paletteGlassIcyBlue = paletteGlass + COLOUR_ICY_BLUE,
    paletteGlassTeal = paletteGlass + COLOUR_TEAL,
    paletteGlassAquamarine = paletteGlass + COLOUR_AQUAMARINE,
    paletteGlassSaturatedGreen = paletteGlass + COLOUR_SATURATED_GREEN,
    paletteGlassDarkGreen = paletteGlass + COLOUR_DARK_GREEN,
    paletteGlassMossGreen = paletteGlass + COLOUR_MOSS_GREEN,
    paletteGlassBrightGreen = paletteGlass + COLOUR_BRIGHT_GREEN,
    paletteGlassOliveGreen = paletteGlass + COLOUR_OLIVE_GREEN,
    paletteGlassDarkOliveGreen = paletteGlass + COLOUR_DARK_OLIVE_GREEN,
    paletteGlassBrightYellow = paletteGlass + COLOUR_BRIGHT_YELLOW,
    paletteGlassYellow = paletteGlass + COLOUR_YELLOW,
    paletteGlassDarkYellow = paletteGlass + COLOUR_DARK_YELLOW,
    paletteGlassLightOrange = paletteGlass + COLOUR_LIGHT_ORANGE,
    paletteGlassDarkOrange = paletteGlass + COLOUR_DARK_ORANGE,
    paletteGlassLightBrown = paletteGlass + COLOUR_LIGHT_BROWN,
    paletteGlassSaturatedBrown = paletteGlass + COLOUR_SATURATED_BROWN,
    paletteGlassDarkBrown = paletteGlass + COLOUR_DARK_BROWN,
    paletteGlassSalmonPink = paletteGlass + COLOUR_SALMON_PINK,
    paletteGlassBordeauxRed = paletteGlass + COLOUR_BORDEAUX_RED,
    paletteGlassSaturatedRed = paletteGlass + COLOUR_SATURATED_RED,
    paletteGlassBrightRed = paletteGlass + COLOUR_BRIGHT_RED,
    paletteGlassDarkPink = paletteGlass + COLOUR_DARK_PINK,
    paletteGlassBrightPink = paletteGlass + COLOUR_BRIGHT_PINK,
    paletteGlassLightPink = paletteGlass + COLOUR_LIGHT_PINK,
    paletteGlassDarkOliveDark = paletteGlass + COLOUR_DARK_OLIVE_DARK,
    paletteGlassDarkOliveLight = paletteGlass + COLOUR_DARK_OLIVE_LIGHT,
    paletteGlassSaturatedBrownLight = paletteGlass + COLOUR_SATURATED_BROWN_LIGHT,
    paletteGlassBordeauxRedDark = paletteGlass + COLOUR_BORDEAUX_RED_DARK,
    paletteGlassBordeauxRedLight = paletteGlass + COLOUR_BORDEAUX_RED_LIGHT,
    paletteGlassGrassGreenDark = paletteGlass + COLOUR_GRASS_GREEN_DARK,
    paletteGlassGrassGreenLight = paletteGlass + COLOUR_GRASS_GREEN_LIGHT,
    paletteGlassOliveDark = paletteGlass + COLOUR_OLIVE_DARK,
    paletteGlassOliveLight = paletteGlass + COLOUR_OLIVE_LIGHT,
    paletteGlassSaturatedGreenLight = paletteGlass + COLOUR_SATURATED_GREEN_LIGHT,
    paletteGlassTanDark = paletteGlass + COLOUR_TAN_DARK,
    paletteGlassTanLight = paletteGlass + COLOUR_TAN_LIGHT,
    paletteGlassDullPurpleLight = paletteGlass + COLOUR_DULL_PURPLE_LIGHT,
    paletteGlassDullGreenDark = paletteGlass + COLOUR_DULL_GREEN_DARK,
    paletteGlassDullGreenLight = paletteGlass + COLOUR_DULL_GREEN_LIGHT,
    paletteGlassSaturatedPurpleDark = paletteGlass + COLOUR_SATURATED_PURPLE_DARK,
    paletteGlassSaturatedPurpleLight = paletteGlass + COLOUR_SATURATED_PURPLE_LIGHT,
    paletteGlassOrangeLight = paletteGlass + COLOUR_ORANGE_LIGHT,
    paletteGlassAquaDark = paletteGlass + COLOUR_AQUA_DARK,
    paletteGlassMagentaLight = paletteGlass + COLOUR_MAGENTA_LIGHT,
    paletteGlassDullBrownDark = paletteGlass + COLOUR_DULL_BROWN_DARK,
    paletteGlassDullBrownLight = paletteGlass + COLOUR_DULL_BROWN_LIGHT,
    paletteGlassInvisible = paletteGlass + COLOUR_INVISIBLE,
    paletteGlassVoid = paletteGlass + COLOUR_VOID,
};

struct TranslucentWindowPalette
{
    FilterPaletteID base;
    FilterPaletteID highlight;
    FilterPaletteID shadow;
};

/**
 * Represents an 8-bit indexed map that maps from one palette index to another.
 */
struct PaletteMap
{
private:
    std::span<uint8_t> _data{};
#ifdef _DEBUG
    // We only require those fields for the asserts in debug builds.
    size_t _numMaps{};
    size_t _mapLength{};
#endif

public:
    static PaletteMap GetDefault();

    constexpr PaletteMap() = default;

    constexpr PaletteMap(uint8_t* data, size_t numMaps, size_t mapLength)
        : _data{ data, numMaps * mapLength }
#ifdef _DEBUG
        , _numMaps(numMaps)
        , _mapLength(mapLength)
#endif
    {
    }

    constexpr PaletteMap(std::span<uint8_t> map)
        : _data(map)
#ifdef _DEBUG
        , _numMaps(1)
        , _mapLength(map.size())
#endif
    {
    }

    uint8_t& operator[](size_t index);
    uint8_t operator[](size_t index) const;

    uint8_t Blend(uint8_t src, uint8_t dst) const;
    void Copy(size_t dstIndex, const PaletteMap& src, size_t srcIndex, size_t length);
};

struct DrawSpriteArgs
{
    ImageId Image;
    PaletteMap PalMap;
    const G1Element& SourceImage;
    int32_t SrcX;
    int32_t SrcY;
    int32_t Width;
    int32_t Height;
    uint8_t* DestinationBits;

    DrawSpriteArgs(
        ImageId image, const PaletteMap& palMap, const G1Element& sourceImage, int32_t srcX, int32_t srcY, int32_t width,
        int32_t height, uint8_t* destinationBits)
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
bool FASTCALL BlitPixel(const uint8_t* src, uint8_t* dst, const PaletteMap& paletteMap)
{
    if constexpr (TBlendOp & kBlendTransparent)
    {
        // Ignore transparent pixels
        if (*src == 0)
        {
            return false;
        }
    }

    if constexpr (((TBlendOp & kBlendSrc) != 0) && ((TBlendOp & kBlendDst) != 0))
    {
        auto pixel = paletteMap.Blend(*src, *dst);
        if constexpr (TBlendOp & kBlendTransparent)
        {
            if (pixel == 0)
            {
                return false;
            }
        }
        *dst = pixel;
        return true;
    }
    else if constexpr ((TBlendOp & kBlendSrc) != 0)
    {
        auto pixel = paletteMap[*src];
        if constexpr (TBlendOp & kBlendTransparent)
        {
            if (pixel == 0)
            {
                return false;
            }
        }
        *dst = pixel;
        return true;
    }
    else if constexpr ((TBlendOp & kBlendDst) != 0)
    {
        auto pixel = paletteMap[*dst];
        if constexpr (TBlendOp & kBlendTransparent)
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

constexpr int8_t kMaxScrollingTextModes = 38;

extern OpenRCT2::Drawing::GamePalette gPalette;
extern OpenRCT2::Drawing::GamePalette gGamePalette;
extern uint32_t gPaletteEffectFrame;

extern uint8_t gTextPalette[];
extern const TranslucentWindowPalette kTranslucentWindowPalettes[COLOUR_COUNT];

extern ImageId gPickupPeepImage;
extern int32_t gPickupPeepX;
extern int32_t gPickupPeepY;
extern bool gPaintForceRedraw;

bool ClipDrawPixelInfo(RenderTarget& dst, RenderTarget& src, const ScreenCoordsXY& coords, int32_t width, int32_t height);
void GfxSetDirtyBlocks(const ScreenRect& rect);
void GfxInvalidateScreen();

// palette
void GfxTransposePalette(int32_t pal, uint8_t product);
void LoadPalette();

// other
void GfxClear(RenderTarget& rt, uint8_t paletteIndex);
void GfxFilterPixel(RenderTarget& rt, const ScreenCoordsXY& coords, FilterPaletteID palette);
void GfxInvalidatePickedUpPeep();
void GfxDrawPickedUpPeep(RenderTarget& rt);

// line
void GfxDrawLine(RenderTarget& rt, const ScreenLine& line, int32_t colour);
void GfxDrawLineSoftware(RenderTarget& rt, const ScreenLine& line, int32_t colour);
void GfxDrawDashedLine(
    RenderTarget& rt, const ScreenLine& screenLine, const int32_t dashedLineSegmentLength, const int32_t color);

// sprite
bool GfxLoadG1(const OpenRCT2::IPlatformEnvironment& env);
void GfxLoadG2FontsAndTracks();
bool GfxLoadCsg();
void GfxUnloadG1();
void GfxUnloadG2AndFonts();
void GfxUnloadCsg();
const G1Element* GfxGetG1Element(const ImageId imageId);
const G1Element* GfxGetG1Element(ImageIndex image_id);
void GfxSetG1Element(ImageIndex imageId, const G1Element* g1);
std::optional<Gx> GfxLoadGx(const std::vector<uint8_t>& buffer);
bool IsCsgLoaded();
void FASTCALL GfxSpriteToBuffer(RenderTarget& rt, const DrawSpriteArgs& args);
void FASTCALL GfxBmpSpriteToBuffer(RenderTarget& rt, const DrawSpriteArgs& args);
void FASTCALL GfxRleSpriteToBuffer(RenderTarget& rt, const DrawSpriteArgs& args);
void FASTCALL GfxDrawSprite(RenderTarget& rt, const ImageId image_id, const ScreenCoordsXY& coords);
void FASTCALL GfxDrawGlyph(RenderTarget& rt, const ImageId image, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL GfxDrawSpriteSolid(RenderTarget& rt, const ImageId image, const ScreenCoordsXY& coords, uint8_t colour);
void FASTCALL
    GfxDrawSpriteRawMasked(RenderTarget& rt, const ScreenCoordsXY& coords, const ImageId maskImage, const ImageId colourImage);
void FASTCALL GfxDrawSpriteSoftware(RenderTarget& rt, const ImageId imageId, const ScreenCoordsXY& spriteCoords);
void FASTCALL GfxDrawSpritePaletteSetSoftware(
    RenderTarget& rt, const ImageId imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL GfxDrawSpriteRawMaskedSoftware(
    RenderTarget& rt, const ScreenCoordsXY& scrCoords, const ImageId maskImage, const ImageId colourImage);

// string
void GfxDrawStringLeftCentred(
    RenderTarget& rt, StringId format, void* args, ColourWithFlags colour, const ScreenCoordsXY& coords);
void DrawStringCentredRaw(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t numLines, const utf8* text, FontStyle fontStyle);
void DrawNewsTicker(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, colour_t colour, StringId format, u8string_view args,
    int32_t ticks);
void GfxDrawStringWithYOffsets(
    RenderTarget& rt, const utf8* text, ColourWithFlags colour, const ScreenCoordsXY& coords, const int8_t* yOffsets,
    bool forceSpriteFont, FontStyle fontStyle);

int32_t GfxWrapString(u8string_view text, int32_t width, FontStyle fontStyle, u8string* outWrappedText, int32_t* outNumLines);
int32_t GfxGetStringWidth(std::string_view text, FontStyle fontStyle);
int32_t GfxGetStringWidthNewLined(std::string_view text, FontStyle fontStyle);
int32_t GfxGetStringWidthNoFormatting(std::string_view text, FontStyle fontStyle);
int32_t StringGetHeightRaw(std::string_view text, FontStyle fontStyle);
int32_t GfxClipString(char* buffer, int32_t width, FontStyle fontStyle);
u8string ShortenPath(const u8string& path, int32_t availableWidth, FontStyle fontStyle);
void TTFDrawString(
    RenderTarget& rt, const_utf8string text, ColourWithFlags colour, const ScreenCoordsXY& coords, bool noFormatting,
    FontStyle fontStyle, TextDarkness darkness);

// scrolling text
void ScrollingTextInitialiseBitmaps();
void ScrollingTextInvalidate();

namespace OpenRCT2
{
    class Formatter;
}

ImageId ScrollingTextSetup(
    struct PaintSession& session, StringId stringId, OpenRCT2::Formatter& ft, uint16_t scroll, uint16_t scrollingMode,
    colour_t colour);

size_t G1CalculateDataSize(const G1Element* g1);

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

std::optional<uint32_t> GetPaletteG1Index(colour_t paletteId);
std::optional<PaletteMap> GetPaletteMapForColour(colour_t paletteId);
void UpdatePalette(std::span<const OpenRCT2::Drawing::PaletteBGRA> palette, int32_t start_index, int32_t num_colours);
void UpdatePaletteEffects();

void RefreshVideo();
void ToggleWindowedMode();

void DebugDPI(RenderTarget& rt);

#include "NewDrawing.h"
