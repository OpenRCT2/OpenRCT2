/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"
#include "../interface/Colour.h"
#include "../interface/ZoomLevel.h"
#include "../world/Location.hpp"
#include "Font.h"
#include "ImageId.hpp"
#include "Text.h"

#include <memory>
#include <optional>
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

struct PaletteBGRA
{
    uint8_t Blue{};
    uint8_t Green{};
    uint8_t Red{};
    uint8_t Alpha{};
};

constexpr const auto PALETTE_SIZE = 256;

struct GamePalette
{
    PaletteBGRA Colour[PALETTE_SIZE]{};

    PaletteBGRA& operator[](uint16_t idx)
    {
        assert(idx < PALETTE_SIZE);
        if (idx >= PALETTE_SIZE)
        {
            static PaletteBGRA dummy;
            return dummy;
        }

        return Colour[idx];
    }

    const PaletteBGRA operator[](uint16_t idx) const
    {
        assert(idx < PALETTE_SIZE);
        if (idx >= PALETTE_SIZE)
            return {};

        return Colour[idx];
    }

    explicit operator uint8_t*()
    {
        return reinterpret_cast<uint8_t*>(Colour);
    }
};

struct rct_g1_element
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
struct rct_g1_header
{
    uint32_t num_entries = 0;
    uint32_t total_size = 0;
};
assert_struct_size(rct_g1_header, 8);
#pragma pack(pop)

struct rct_gx
{
    rct_g1_header header;
    std::vector<rct_g1_element> elements;
    std::unique_ptr<uint8_t[]> data;
};

struct rct_drawpixelinfo
{
    uint8_t* bits{};
    int32_t x{};
    int32_t y{};
    int32_t width{};
    int32_t height{};
    int32_t pitch{}; // note: this is actually (pitch - width)
    ZoomLevel zoom_level{};

    /**
     * As x and y are based on 1:1 units, zooming in will cause a reduction in precision when mapping zoomed-in
     * pixels to 1:1 pixels. When x, y are not a multiple of the zoom level, the remainder will be non-zero.
     * The drawing of sprites will need to be offset by this amount.
     */
    uint8_t remX{};
    uint8_t remY{};

    // Last position of drawn text.
    ScreenCoordsXY lastStringPos{};

    OpenRCT2::Drawing::IDrawingEngine* DrawingEngine{};

    size_t GetBytesPerRow() const;
    uint8_t* GetBitsOffset(const ScreenCoordsXY& pos) const;
    rct_drawpixelinfo Crop(const ScreenCoordsXY& pos, const ScreenSize& size) const;
};

struct rct_g1_element_32bit
{
    uint32_t offset;        // 0x00 note: uint32_t always!
    int16_t width;          // 0x04
    int16_t height;         // 0x06
    int16_t x_offset;       // 0x08
    int16_t y_offset;       // 0x0A
    uint16_t flags;         // 0x0C
    uint16_t zoomed_offset; // 0x0E
};
assert_struct_size(rct_g1_element_32bit, 0x10);

enum
{
    G1_FLAG_HAS_TRANSPARENCY = (1 << 0), // Image data contains transparent pixels (0XFF) which will not be rendered
    G1_FLAG_1 = (1 << 1),
    G1_FLAG_RLE_COMPRESSION = (1 << 2), // Image data is encoded using RCT2's form of run length encoding
    G1_FLAG_PALETTE = (1 << 3),         // Image data is a sequence of palette entries R8G8B8
    G1_FLAG_HAS_ZOOM_SPRITE = (1 << 4), // Use a different sprite for higher zoom levels
    G1_FLAG_NO_ZOOM_DRAW = (1 << 5),    // Does not get drawn at higher zoom levels (only zoom 0)
};

enum : uint32_t
{
    IMAGE_TYPE_DEFAULT = 0,
    IMAGE_TYPE_REMAP = (1 << 29),
    IMAGE_TYPE_TRANSPARENT = (1 << 30),
    IMAGE_TYPE_REMAP_2_PLUS = (1u << 31)
    // REMAP_2_PLUS + REMAP = REMAP 2
    // REMAP_2_PLUS = REMAP 3
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
};

enum class FilterPaletteID : int32_t
{
    PaletteNull = 0,

    PaletteWater = 32,

    Palette34 = 34,

    Palette44 = 44, // Construction marker
    Palette45 = 45, // Decolourise + lighten
    Palette46 = 46,

    PaletteDarken3 = 47,

    PaletteDarken1 = 49,
    PaletteDarken2 = 50,
    Palette51 = 51, // Decolourise + darken
    PaletteTranslucentGrey = 52,
    PaletteTranslucentGreyHighlight = 53,
    PaletteTranslucentGreyShadow = 54,
    PaletteTranslucentLightBlue = 55,
    PaletteTranslucentLightBlueHighlight = 56,
    PaletteTranslucentLightBlueShadow = 57,
    PaletteTranslucentBordeauxRed = 58,
    PaletteTranslucentBordeauxRedHighlight = 59,
    PaletteTranslucentBordeauxRedShadow = 60,
    PaletteTranslucentDarkGreen = 61,
    PaletteTranslucentDarkGreenHighlight = 62,
    PaletteTranslucentDarkGreenShadow = 63,
    PaletteTranslucentLightPurple = 64,
    PaletteTranslucentLightPurpleHighlight = 65,
    PaletteTranslucentLightPurpleShadow = 66,
    PaletteTranslucentDarkOliveGreen = 67,
    PaletteTranslucentDarkOliveGreenHighlight = 68,
    PaletteTranslucentDarkOliveGreenShadow = 69,
    PaletteTranslucentLightBrown = 70,
    PaletteTranslucentLightBrownHighlight = 71,
    PaletteTranslucentLightBrownShadow = 72,
    PaletteTranslucentYellow = 73,
    PaletteTranslucentYellowHighlight = 74,
    PaletteTranslucentYellowShadow = 75,
    PaletteTranslucentMossGreen = 76,
    PaletteTranslucentMossGreenHighlight = 77,
    PaletteTranslucentMossGreenShadow = 78,
    PaletteTranslucentOliveGreen = 79,
    PaletteTranslucentOliveGreenHighlight = 80,
    PaletteTranslucentOliveGreenShadow = 81,
    PaletteTranslucentBrightGreen = 82,
    PaletteTranslucentBrightGreenHighlight = 83,
    PaletteTranslucentBrightGreenShadow = 84,
    PaletteTranslucentSalmonPink = 85,
    PaletteTranslucentSalmonPinkHighlight = 86,
    PaletteTranslucentSalmonPinkShadow = 87,
    PaletteTranslucentBrightPurple = 88,
    PaletteTranslucentBrightPurpleHighlight = 89,
    PaletteTranslucentBrightPurpleShadow = 90,
    PaletteTranslucentBrightRed = 91,
    PaletteTranslucentBrightRedHighlight = 92,
    PaletteTranslucentBrightRedShadow = 93,
    PaletteTranslucentLightOrange = 94,
    PaletteTranslucentLightOrangeHighlight = 95,
    PaletteTranslucentLightOrangeShadow = 96,
    PaletteTranslucentTeal = 97,
    PaletteTranslucentTealHighlight = 98,
    PaletteTranslucentTealShadow = 99,
    PaletteTranslucentBrightPink = 100,
    PaletteTranslucentBrightPinkHighlight = 101,
    PaletteTranslucentBrightPinkShadow = 102,
    PaletteTranslucentDarkBrown = 103,
    PaletteTranslucentDarkBrownHighlight = 104,
    PaletteTranslucentDarkBrownShadow = 105,
    PaletteTranslucentLightPink = 106,
    PaletteTranslucentLightPinkHighlight = 107,
    PaletteTranslucentLightPinkShadow = 108,
    PaletteTranslucentWhite = 109,
    PaletteTranslucentWhiteHighlight = 110,
    PaletteTranslucentWhiteShadow = 111,
    PaletteGlass = 112,
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
};

struct translucent_window_palette
{
    FilterPaletteID base;
    FilterPaletteID highlight;
    FilterPaletteID shadow;
};

struct rct_size16
{
    int16_t width;
    int16_t height;
};

/**
 * Represents an 8-bit indexed map that maps from one palette index to another.
 */
struct PaletteMap
{
private:
    uint8_t* _data{};
    uint32_t _dataLength{};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint16_t _numMaps;
#pragma clang diagnostic pop
    uint16_t _mapLength;

public:
    static const PaletteMap& GetDefault();

    PaletteMap() = default;

    PaletteMap(uint8_t* data, uint16_t numMaps, uint16_t mapLength)
        : _data(data)
        , _dataLength(numMaps * mapLength)
        , _numMaps(numMaps)
        , _mapLength(mapLength)
    {
    }

    template<std::size_t TSize>
    PaletteMap(uint8_t (&map)[TSize])
        : _data(map)
        , _dataLength(static_cast<uint32_t>(std::size(map)))
        , _numMaps(1)
        , _mapLength(static_cast<uint16_t>(std::size(map)))
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
    const PaletteMap& PalMap;
    const rct_g1_element& SourceImage;
    int32_t SrcX;
    int32_t SrcY;
    int32_t Width;
    int32_t Height;
    uint8_t* DestinationBits;

    DrawSpriteArgs(
        ImageId image, const PaletteMap& palMap, const rct_g1_element& sourceImage, int32_t srcX, int32_t srcY, int32_t width,
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

template<DrawBlendOp TBlendOp> bool FASTCALL BlitPixel(const uint8_t* src, uint8_t* dst, const PaletteMap& paletteMap)
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

#define SPRITE_ID_PALETTE_COLOUR_1(colourId) (IMAGE_TYPE_REMAP | ((colourId) << 19))

#define PALETTE_TO_G1_OFFSET_COUNT 144

#define INSET_RECT_F_30 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE)
#define INSET_RECT_F_60 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN)
#define INSET_RECT_F_E0 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN | INSET_RECT_FLAG_FILL_MID_LIGHT)

#define MAX_SCROLLING_TEXT_MODES 38

extern GamePalette gPalette;
extern uint8_t gGamePalette[256 * 4];
extern uint32_t gPaletteEffectFrame;
extern const FilterPaletteID GlassPaletteIds[COLOUR_COUNT];
extern thread_local uint8_t gPeepPalette[256];
extern thread_local uint8_t gOtherPalette[256];
extern uint8_t gTextPalette[];
extern const translucent_window_palette TranslucentWindowPalettes[COLOUR_COUNT];

extern ImageId gPickupPeepImage;
extern int32_t gPickupPeepX;
extern int32_t gPickupPeepY;

extern bool gTinyFontAntiAliased;

bool clip_drawpixelinfo(
    rct_drawpixelinfo* dst, rct_drawpixelinfo* src, const ScreenCoordsXY& coords, int32_t width, int32_t height);
void gfx_set_dirty_blocks(const ScreenRect& rect);
void gfx_invalidate_screen();

// palette
void gfx_transpose_palette(int32_t pal, uint8_t product);
void load_palette();

// other
void gfx_clear(rct_drawpixelinfo* dpi, uint8_t paletteIndex);
void gfx_filter_pixel(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, FilterPaletteID palette);
void gfx_invalidate_pickedup_peep();
void gfx_draw_pickedup_peep(rct_drawpixelinfo* dpi);

// line
void gfx_draw_line(rct_drawpixelinfo* dpi, const ScreenLine& line, int32_t colour);
void gfx_draw_line_software(rct_drawpixelinfo* dpi, const ScreenLine& line, int32_t colour);
void gfx_draw_dashed_line(
    rct_drawpixelinfo* dpi, const ScreenLine& screenLine, const int32_t dashedLineSegmentLength, const int32_t color);

// rect
void gfx_fill_rect(rct_drawpixelinfo* dpi, const ScreenRect& rect, int32_t colour);
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, const ScreenRect& rect, int32_t colour, uint8_t flags);
void gfx_filter_rect(rct_drawpixelinfo* dpi, const ScreenRect& rect, FilterPaletteID palette);

// sprite
bool gfx_load_g1(const OpenRCT2::IPlatformEnvironment& env);
bool gfx_load_g2();
bool gfx_load_csg();
void gfx_unload_g1();
void gfx_unload_g2();
void gfx_unload_csg();
const rct_g1_element* gfx_get_g1_element(const ImageId& imageId);
const rct_g1_element* gfx_get_g1_element(ImageIndex image_id);
void gfx_set_g1_element(ImageIndex imageId, const rct_g1_element* g1);
std::optional<rct_gx> GfxLoadGx(const std::vector<uint8_t>& buffer);
bool is_csg_loaded();
void FASTCALL gfx_sprite_to_buffer(rct_drawpixelinfo& dpi, const DrawSpriteArgs& args);
void FASTCALL gfx_bmp_sprite_to_buffer(rct_drawpixelinfo& dpi, const DrawSpriteArgs& args);
void FASTCALL gfx_rle_sprite_to_buffer(rct_drawpixelinfo& dpi, const DrawSpriteArgs& args);
void FASTCALL gfx_draw_sprite(rct_drawpixelinfo* dpi, const ImageId& image_id, const ScreenCoordsXY& coords);
void FASTCALL
    gfx_draw_glyph(rct_drawpixelinfo* dpi, int32_t image_id, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL gfx_draw_sprite_solid(rct_drawpixelinfo* dpi, const ImageId& image, const ScreenCoordsXY& coords, uint8_t colour);
void FASTCALL gfx_draw_sprite_raw_masked(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const ImageId& maskImage, const ImageId& colourImage);
void FASTCALL gfx_draw_sprite_software(rct_drawpixelinfo* dpi, const ImageId& imageId, const ScreenCoordsXY& spriteCoords);
void FASTCALL gfx_draw_sprite_palette_set_software(
    rct_drawpixelinfo* dpi, const ImageId& imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL gfx_draw_sprite_raw_masked_software(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& scrCoords, const ImageId& maskImage, const ImageId& colourImage);

// string
void gfx_draw_string(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const_utf8string buffer, TextPaint textPaint = {});
void gfx_draw_string_no_formatting(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const_utf8string buffer, TextPaint textPaint);

void gfx_draw_string_left_centred(
    rct_drawpixelinfo* dpi, StringId format, void* args, colour_t colour, const ScreenCoordsXY& coords);
void draw_string_centred_raw(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t numLines, char* text, FontSpriteBase fontSpriteBase);
void DrawNewsTicker(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, colour_t colour, StringId format, void* args,
    int32_t ticks);
void gfx_draw_string_with_y_offsets(
    rct_drawpixelinfo* dpi, const utf8* text, int32_t colour, const ScreenCoordsXY& coords, const int8_t* yOffsets,
    bool forceSpriteFont, FontSpriteBase fontSpriteBase);

int32_t gfx_wrap_string(char* buffer, int32_t width, FontSpriteBase fontSpriteBase, int32_t* num_lines);
int32_t gfx_get_string_width(std::string_view text, FontSpriteBase fontSpriteBase);
int32_t gfx_get_string_width_new_lined(std::string_view text, FontSpriteBase fontSpriteBase);
int32_t gfx_get_string_width_no_formatting(std::string_view text, FontSpriteBase fontSpriteBase);
int32_t string_get_height_raw(std::string_view text, FontSpriteBase fontBase);
int32_t gfx_clip_string(char* buffer, int32_t width, FontSpriteBase fontSpriteBase);
void shorten_path(utf8* buffer, size_t bufferSize, const utf8* path, int32_t availableWidth, FontSpriteBase fontSpriteBase);
void ttf_draw_string(
    rct_drawpixelinfo* dpi, const_utf8string text, int32_t colour, const ScreenCoordsXY& coords, bool noFormatting,
    FontSpriteBase fontSpriteBase);

// scrolling text
void scrolling_text_initialise_bitmaps();
void scrolling_text_invalidate();

class Formatter;

ImageId scrolling_text_setup(
    struct PaintSession& session, StringId stringId, Formatter& ft, uint16_t scroll, uint16_t scrollingMode, colour_t colour);

rct_size16 FASTCALL gfx_get_sprite_size(uint32_t image_id);
size_t g1_calculate_data_size(const rct_g1_element* g1);

void mask_scalar(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
void mask_sse4_1(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
void mask_avx2(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
void mask_init();

extern void (*mask_fn)(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);

std::optional<uint32_t> GetPaletteG1Index(colour_t paletteId);
std::optional<PaletteMap> GetPaletteMapForColour(colour_t paletteId);
void UpdatePalette(const uint8_t* colours, int32_t start_index, int32_t num_colours);

void RefreshVideo(bool recreateWindow);
void ToggleWindowedMode();

#include "NewDrawing.h"
