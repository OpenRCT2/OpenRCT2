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

struct DrawPixelInfo
{
    uint8_t* bits{};
    int32_t x{};
    int32_t y{};
    int32_t width{};
    int32_t height{};
    int32_t pitch{}; // note: this is actually (pitch - width)
    ZoomLevel zoom_level{};

    // Last position of drawn text.
    ScreenCoordsXY lastStringPos{};

    OpenRCT2::Drawing::IDrawingEngine* DrawingEngine{};

    uint8_t* GetBitsOffset(const ScreenCoordsXY& pos) const;
    DrawPixelInfo Crop(const ScreenCoordsXY& pos, const ScreenSize& size) const;

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

struct TextDrawInfo
{
    int32_t startX;
    int32_t startY;
    int32_t x;
    int32_t y;
    int32_t maxX;
    int32_t maxY;
    int32_t flags;
    uint8_t palette[8];
    ::FontStyle FontStyle;
    const int8_t* y_offset;
};

enum : uint32_t
{
    TEXT_DRAW_FLAG_INSET = 1 << 0,
    TEXT_DRAW_FLAG_OUTLINE = 1 << 1,
    TEXT_DRAW_FLAG_DARK = 1 << 2,
    TEXT_DRAW_FLAG_EXTRA_DARK = 1 << 3,
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

constexpr DrawBlendOp BLEND_NONE = 0;

/**
 * Only supported by BITMAP. RLE images always encode transparency via the encoding.
 * Pixel value of 0 represents transparent.
 */
constexpr DrawBlendOp BLEND_TRANSPARENT = 1 << 0;

/**
 * Whether to use the pixel value from the source image.
 * This is usually only unset for glass images where there the src is only a transparency mask.
 */
constexpr DrawBlendOp BLEND_SRC = 1 << 1;

/**
 * Whether to use the pixel value of the destination image for blending.
 * This is used for any image that filters the target image, e.g. glass or water.
 */
constexpr DrawBlendOp BLEND_DST = 2 << 2;

enum
{
    INSET_RECT_FLAG_FILL_GREY = (1 << 2),         // 0x04
    INSET_RECT_FLAG_BORDER_NONE = (1 << 3),       // 0x08
    INSET_RECT_FLAG_FILL_NONE = (1 << 4),         // 0x10
    INSET_RECT_FLAG_BORDER_INSET = (1 << 5),      // 0x20
    INSET_RECT_FLAG_FILL_DONT_LIGHTEN = (1 << 6), // 0x40
    INSET_RECT_FLAG_FILL_MID_LIGHT = (1 << 7),    // 0x80

    INSET_RECT_F_30 = (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE),
    INSET_RECT_F_60 = (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN),
    INSET_RECT_F_E0 = (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN | INSET_RECT_FLAG_FILL_MID_LIGHT),
};

enum class FilterPaletteID : int32_t
{
    PaletteNull = 0,

    PaletteWater = COLOUR_COUNT,

    PaletteLandMarker0, // North (corner/edge)
    PaletteLandMarker1, // East (corner/edge)
    PaletteLandMarker2, // South (corner/edge)
    PaletteLandMarker3, // West (corner/edge)
    PaletteSceneryGroundMarker,
    PaletteWaterMarker,
    PaletteQuarterMarker0, // North (not sure why it couldn't just use PaletteLandMarker0)
    PaletteQuarterMarker1, // East
    PaletteQuarterMarker2, // South
    PaletteQuarterMarker3, // West
    PaletteRideGroundMarker,
    PaletteGhost, // Construction marker
    Palette45,    // Decolourise + lighten
    Palette46,

    PaletteDarken3,

    PaletteDarken1 = PaletteDarken3 + 2,
    PaletteDarken2,
    Palette51, // Decolourise + darken
    PaletteTranslucentGrey,
    PaletteTranslucentGreyHighlight,
    PaletteTranslucentGreyShadow,
    PaletteTranslucentLightBlue,
    PaletteTranslucentLightBlueHighlight,
    PaletteTranslucentLightBlueShadow,
    PaletteTranslucentBordeauxRed,
    PaletteTranslucentBordeauxRedHighlight,
    PaletteTranslucentBordeauxRedShadow,
    PaletteTranslucentDarkGreen,
    PaletteTranslucentDarkGreenHighlight,
    PaletteTranslucentDarkGreenShadow,
    PaletteTranslucentLightPurple,
    PaletteTranslucentLightPurpleHighlight,
    PaletteTranslucentLightPurpleShadow,
    PaletteTranslucentDarkOliveGreen,
    PaletteTranslucentDarkOliveGreenHighlight,
    PaletteTranslucentDarkOliveGreenShadow,
    PaletteTranslucentLightBrown,
    PaletteTranslucentLightBrownHighlight,
    PaletteTranslucentLightBrownShadow,
    PaletteTranslucentYellow,
    PaletteTranslucentYellowHighlight,
    PaletteTranslucentYellowShadow,
    PaletteTranslucentMossGreen,
    PaletteTranslucentMossGreenHighlight,
    PaletteTranslucentMossGreenShadow,
    PaletteTranslucentOliveGreen,
    PaletteTranslucentOliveGreenHighlight,
    PaletteTranslucentOliveGreenShadow,
    PaletteTranslucentBrightGreen,
    PaletteTranslucentBrightGreenHighlight,
    PaletteTranslucentBrightGreenShadow,
    PaletteTranslucentSalmonPink,
    PaletteTranslucentSalmonPinkHighlight,
    PaletteTranslucentSalmonPinkShadow,
    PaletteTranslucentBrightPurple,
    PaletteTranslucentBrightPurpleHighlight,
    PaletteTranslucentBrightPurpleShadow,
    PaletteTranslucentBrightRed,
    PaletteTranslucentBrightRedHighlight,
    PaletteTranslucentBrightRedShadow,
    PaletteTranslucentLightOrange,
    PaletteTranslucentLightOrangeHighlight,
    PaletteTranslucentLightOrangeShadow,
    PaletteTranslucentTeal,
    PaletteTranslucentTealHighlight,
    PaletteTranslucentTealShadow,
    PaletteTranslucentBrightPink,
    PaletteTranslucentBrightPinkHighlight,
    PaletteTranslucentBrightPinkShadow,
    PaletteTranslucentDarkBrown,
    PaletteTranslucentDarkBrownHighlight,
    PaletteTranslucentDarkBrownShadow,
    PaletteTranslucentLightPink,
    PaletteTranslucentLightPinkHighlight,
    PaletteTranslucentLightPinkShadow,
    PaletteTranslucentWhite,
    PaletteTranslucentWhiteHighlight,
    PaletteTranslucentWhiteShadow,
    PaletteGlass,
    PaletteGlassBlack = PaletteGlass + COLOUR_BLACK,
    PaletteGlassGrey = PaletteGlass + COLOUR_GREY,
    PaletteGlassWhite = PaletteGlass + COLOUR_WHITE,
    PaletteGlassDarkPurple = PaletteGlass + COLOUR_DARK_PURPLE,
    PaletteGlassLightPurple = PaletteGlass + COLOUR_LIGHT_PURPLE,
    PaletteGlassBrightPurple = PaletteGlass + COLOUR_BRIGHT_PURPLE,
    PaletteGlassDarkBlue = PaletteGlass + COLOUR_DARK_BLUE,
    PaletteGlassLightBlue = PaletteGlass + COLOUR_LIGHT_BLUE,
    PaletteGlassIcyBlue = PaletteGlass + COLOUR_ICY_BLUE,
    PaletteGlassTeal = PaletteGlass + COLOUR_TEAL,
    PaletteGlassAquamarine = PaletteGlass + COLOUR_AQUAMARINE,
    PaletteGlassSaturatedGreen = PaletteGlass + COLOUR_SATURATED_GREEN,
    PaletteGlassDarkGreen = PaletteGlass + COLOUR_DARK_GREEN,
    PaletteGlassMossGreen = PaletteGlass + COLOUR_MOSS_GREEN,
    PaletteGlassBrightGreen = PaletteGlass + COLOUR_BRIGHT_GREEN,
    PaletteGlassOliveGreen = PaletteGlass + COLOUR_OLIVE_GREEN,
    PaletteGlassDarkOliveGreen = PaletteGlass + COLOUR_DARK_OLIVE_GREEN,
    PaletteGlassBrightYellow = PaletteGlass + COLOUR_BRIGHT_YELLOW,
    PaletteGlassYellow = PaletteGlass + COLOUR_YELLOW,
    PaletteGlassDarkYellow = PaletteGlass + COLOUR_DARK_YELLOW,
    PaletteGlassLightOrange = PaletteGlass + COLOUR_LIGHT_ORANGE,
    PaletteGlassDarkOrange = PaletteGlass + COLOUR_DARK_ORANGE,
    PaletteGlassLightBrown = PaletteGlass + COLOUR_LIGHT_BROWN,
    PaletteGlassSaturatedBrown = PaletteGlass + COLOUR_SATURATED_BROWN,
    PaletteGlassDarkBrown = PaletteGlass + COLOUR_DARK_BROWN,
    PaletteGlassSalmonPink = PaletteGlass + COLOUR_SALMON_PINK,
    PaletteGlassBordeauxRed = PaletteGlass + COLOUR_BORDEAUX_RED,
    PaletteGlassSaturatedRed = PaletteGlass + COLOUR_SATURATED_RED,
    PaletteGlassBrightRed = PaletteGlass + COLOUR_BRIGHT_RED,
    PaletteGlassDarkPink = PaletteGlass + COLOUR_DARK_PINK,
    PaletteGlassBrightPink = PaletteGlass + COLOUR_BRIGHT_PINK,
    PaletteGlassLightPink = PaletteGlass + COLOUR_LIGHT_PINK,
    PaletteGlassDarkOliveDark = PaletteGlass + COLOUR_DARK_OLIVE_DARK,
    PaletteGlassDarkOliveLight = PaletteGlass + COLOUR_DARK_OLIVE_LIGHT,
    PaletteGlassSaturatedBrownLight = PaletteGlass + COLOUR_SATURATED_BROWN_LIGHT,
    PaletteGlassBordeauxRedDark = PaletteGlass + COLOUR_BORDEAUX_RED_DARK,
    PaletteGlassBordeauxRedLight = PaletteGlass + COLOUR_BORDEAUX_RED_LIGHT,
    PaletteGlassGrassGreenDark = PaletteGlass + COLOUR_GRASS_GREEN_DARK,
    PaletteGlassGrassGreenLight = PaletteGlass + COLOUR_GRASS_GREEN_LIGHT,
    PaletteGlassOliveDark = PaletteGlass + COLOUR_OLIVE_DARK,
    PaletteGlassOliveLight = PaletteGlass + COLOUR_OLIVE_LIGHT,
    PaletteGlassSaturatedGreenLight = PaletteGlass + COLOUR_SATURATED_GREEN_LIGHT,
    PaletteGlassTanDark = PaletteGlass + COLOUR_TAN_DARK,
    PaletteGlassTanLight = PaletteGlass + COLOUR_TAN_LIGHT,
    PaletteGlassDullPurpleLight = PaletteGlass + COLOUR_DULL_PURPLE_LIGHT,
    PaletteGlassDullGreenDark = PaletteGlass + COLOUR_DULL_GREEN_DARK,
    PaletteGlassDullGreenLight = PaletteGlass + COLOUR_DULL_GREEN_LIGHT,
    PaletteGlassSaturatedPurpleDark = PaletteGlass + COLOUR_SATURATED_PURPLE_DARK,
    PaletteGlassSaturatedPurpleLight = PaletteGlass + COLOUR_SATURATED_PURPLE_LIGHT,
    PaletteGlassOrangeLight = PaletteGlass + COLOUR_ORANGE_LIGHT,
    PaletteGlassAquaDark = PaletteGlass + COLOUR_AQUA_DARK,
    PaletteGlassMagentaLight = PaletteGlass + COLOUR_MAGENTA_LIGHT,
    PaletteGlassDullBrownDark = PaletteGlass + COLOUR_DULL_BROWN_DARK,
    PaletteGlassDullBrownLight = PaletteGlass + COLOUR_DULL_BROWN_LIGHT,
    PaletteGlassInvisible = PaletteGlass + COLOUR_INVISIBLE,
    PaletteGlassVoid = PaletteGlass + COLOUR_VOID,
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
extern const TranslucentWindowPalette TranslucentWindowPalettes[COLOUR_COUNT];

extern ImageId gPickupPeepImage;
extern int32_t gPickupPeepX;
extern int32_t gPickupPeepY;

extern bool gTinyFontAntiAliased;

bool ClipDrawPixelInfo(DrawPixelInfo& dst, DrawPixelInfo& src, const ScreenCoordsXY& coords, int32_t width, int32_t height);
void GfxSetDirtyBlocks(const ScreenRect& rect);
void GfxInvalidateScreen();

// palette
void GfxTransposePalette(int32_t pal, uint8_t product);
void LoadPalette();

// other
void GfxClear(DrawPixelInfo& dpi, uint8_t paletteIndex);
void GfxFilterPixel(DrawPixelInfo& dpi, const ScreenCoordsXY& coords, FilterPaletteID palette);
void GfxInvalidatePickedUpPeep();
void GfxDrawPickedUpPeep(DrawPixelInfo& dpi);

// line
void GfxDrawLine(DrawPixelInfo& dpi, const ScreenLine& line, int32_t colour);
void GfxDrawLineSoftware(DrawPixelInfo& dpi, const ScreenLine& line, int32_t colour);
void GfxDrawDashedLine(
    DrawPixelInfo& dpi, const ScreenLine& screenLine, const int32_t dashedLineSegmentLength, const int32_t color);

// rect
void GfxFillRect(DrawPixelInfo& dpi, const ScreenRect& rect, int32_t colour);
void GfxFillRectInset(DrawPixelInfo& dpi, const ScreenRect& rect, ColourWithFlags colour, uint8_t flags);
void GfxFilterRect(DrawPixelInfo& dpi, const ScreenRect& rect, FilterPaletteID palette);

// sprite
bool GfxLoadG1(const OpenRCT2::IPlatformEnvironment& env);
bool GfxLoadG2();
bool GfxLoadCsg();
void GfxUnloadG1();
void GfxUnloadG2();
void GfxUnloadCsg();
const G1Element* GfxGetG1Element(const ImageId imageId);
const G1Element* GfxGetG1Element(ImageIndex image_id);
void GfxSetG1Element(ImageIndex imageId, const G1Element* g1);
std::optional<Gx> GfxLoadGx(const std::vector<uint8_t>& buffer);
bool IsCsgLoaded();
void FASTCALL GfxSpriteToBuffer(DrawPixelInfo& dpi, const DrawSpriteArgs& args);
void FASTCALL GfxBmpSpriteToBuffer(DrawPixelInfo& dpi, const DrawSpriteArgs& args);
void FASTCALL GfxRleSpriteToBuffer(DrawPixelInfo& dpi, const DrawSpriteArgs& args);
void FASTCALL GfxDrawSprite(DrawPixelInfo& dpi, const ImageId image_id, const ScreenCoordsXY& coords);
void FASTCALL GfxDrawGlyph(DrawPixelInfo& dpi, const ImageId image, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL GfxDrawSpriteSolid(DrawPixelInfo& dpi, const ImageId image, const ScreenCoordsXY& coords, uint8_t colour);
void FASTCALL GfxDrawSpriteRawMasked(
    DrawPixelInfo& dpi, const ScreenCoordsXY& coords, const ImageId maskImage, const ImageId colourImage);
void FASTCALL GfxDrawSpriteSoftware(DrawPixelInfo& dpi, const ImageId imageId, const ScreenCoordsXY& spriteCoords);
void FASTCALL GfxDrawSpritePaletteSetSoftware(
    DrawPixelInfo& dpi, const ImageId imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL GfxDrawSpriteRawMaskedSoftware(
    DrawPixelInfo& dpi, const ScreenCoordsXY& scrCoords, const ImageId maskImage, const ImageId colourImage);

// string
void GfxDrawStringLeftCentred(
    DrawPixelInfo& dpi, StringId format, void* args, ColourWithFlags colour, const ScreenCoordsXY& coords);
void DrawStringCentredRaw(
    DrawPixelInfo& dpi, const ScreenCoordsXY& coords, int32_t numLines, const utf8* text, FontStyle fontStyle);
void DrawNewsTicker(
    DrawPixelInfo& dpi, const ScreenCoordsXY& coords, int32_t width, colour_t colour, StringId format, u8string_view args,
    int32_t ticks);
void GfxDrawStringWithYOffsets(
    DrawPixelInfo& dpi, const utf8* text, ColourWithFlags colour, const ScreenCoordsXY& coords, const int8_t* yOffsets,
    bool forceSpriteFont, FontStyle fontStyle);

int32_t GfxWrapString(u8string_view text, int32_t width, FontStyle fontStyle, u8string* outWrappedText, int32_t* outNumLines);
int32_t GfxGetStringWidth(std::string_view text, FontStyle fontStyle);
int32_t GfxGetStringWidthNewLined(std::string_view text, FontStyle fontStyle);
int32_t GfxGetStringWidthNoFormatting(std::string_view text, FontStyle fontStyle);
int32_t StringGetHeightRaw(std::string_view text, FontStyle fontStyle);
int32_t GfxClipString(char* buffer, int32_t width, FontStyle fontStyle);
u8string ShortenPath(const u8string& path, int32_t availableWidth, FontStyle fontStyle);
void TTFDrawString(
    DrawPixelInfo& dpi, const_utf8string text, ColourWithFlags colour, const ScreenCoordsXY& coords, bool noFormatting,
    FontStyle fontStyle, TextDarkness darkness);

// scrolling text
void ScrollingTextInitialiseBitmaps();
void ScrollingTextInvalidate();

class Formatter;

ImageId ScrollingTextSetup(
    struct PaintSession& session, StringId stringId, Formatter& ft, uint16_t scroll, uint16_t scrollingMode, colour_t colour);

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

void DebugDPI(DrawPixelInfo& dpi);

#include "NewDrawing.h"
