/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _DRAWING_H_
#define _DRAWING_H_

#include "../common.h"
#include "../interface/Colour.h"
#include "../interface/ZoomLevel.hpp"
#include "../world/Location.hpp"
#include "Text.h"

#include <optional>
#include <vector>

struct ScreenCoordsXY;
struct ScreenLine;
struct ScreenRect;
namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

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
    uint8_t* offset;       // 0x00
    int16_t width;         // 0x04
    int16_t height;        // 0x06
    int16_t x_offset;      // 0x08
    int16_t y_offset;      // 0x0A
    uint16_t flags;        // 0x0C
    int32_t zoomed_offset; // 0x0E
};

#pragma pack(push, 1)
struct rct_g1_header
{
    uint32_t num_entries;
    uint32_t total_size;
};
assert_struct_size(rct_g1_header, 8);
#pragma pack(pop)

struct rct_gx
{
    rct_g1_header header;
    std::vector<rct_g1_element> elements;
    void* data;
};

struct rct_drawpixelinfo
{
    uint8_t* bits{};
    int16_t x{};
    int16_t y{};
    int16_t width{};
    int16_t height{};
    int16_t pitch{}; // note: this is actually (pitch - width)
    ZoomLevel zoom_level{};

    /**
     * As x and y are based on 1:1 units, zooming in will cause a reduction in precision when mapping zoomed-in
     * pixels to 1:1 pixels. When x, y are not a multiple of the zoom level, the remainder will be non-zero.
     * The drawing of sprites will need to be offset by this amount.
     */
    uint8_t remX{};
    uint8_t remY{};

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
    G1_FLAG_BMP = (1 << 0), // Image data is encoded as raw pixels (no transparency)
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

enum FILTER_PALETTE_ID
{
    PALETTE_NULL = 0,

    PALETTE_WATER = 32,

    PALETTE_34 = 34,

    PALETTE_45 = 45, // Decolourise + lighten
    PALETTE_46 = 46,

    PALETTE_DARKEN_3 = 47,

    PALETTE_DARKEN_1 = 49,
    PALETTE_DARKEN_2 = 50,
    PALETTE_51 = 51, // Decolourise + darken
    PALETTE_TRANSLUCENT_GREY = 52,
    PALETTE_TRANSLUCENT_GREY_HIGHLIGHT = 53,
    PALETTE_TRANSLUCENT_GREY_SHADOW = 54,
    PALETTE_TRANSLUCENT_LIGHT_BLUE = 55,
    PALETTE_TRANSLUCENT_LIGHT_BLUE_HIGHLIGHT = 56,
    PALETTE_TRANSLUCENT_LIGHT_BLUE_SHADOW = 57,
    PALETTE_TRANSLUCENT_BORDEAUX_RED = 58,
    PALETTE_TRANSLUCENT_BORDEAUX_RED_HIGHLIGHT = 59,
    PALETTE_TRANSLUCENT_BORDEAUX_RED_SHADOW = 60,
    PALETTE_TRANSLUCENT_DARK_GREEN = 61,
    PALETTE_TRANSLUCENT_DARK_GREEN_HIGHLIGHT = 62,
    PALETTE_TRANSLUCENT_DARK_GREEN_SHADOW = 63,
    PALETTE_TRANSLUCENT_LIGHT_PURPLE = 64,
    PALETTE_TRANSLUCENT_LIGHT_PURPLE_HIGHLIGHT = 65,
    PALETTE_TRANSLUCENT_LIGHT_PURPLE_SHADOW = 66,
    PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN = 67,
    PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN_HIGHLIGHT = 68,
    PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN_SHADOW = 69,
    PALETTE_TRANSLUCENT_LIGHT_BROWN = 70,
    PALETTE_TRANSLUCENT_LIGHT_BROWN_HIGHLIGHT = 71,
    PALETTE_TRANSLUCENT_LIGHT_BROWN_SHADOW = 72,
    PALETTE_TRANSLUCENT_YELLOW = 73,
    PALETTE_TRANSLUCENT_YELLOW_HIGHLIGHT = 74,
    PALETTE_TRANSLUCENT_YELLOW_SHADOW = 75,
    PALETTE_TRANSLUCENT_MOSS_GREEN = 76,
    PALETTE_TRANSLUCENT_MOSS_GREEN_HIGHLIGHT = 77,
    PALETTE_TRANSLUCENT_MOSS_GREEN_SHADOW = 78,
    PALETTE_TRANSLUCENT_OLIVE_GREEN = 79,
    PALETTE_TRANSLUCENT_OLIVE_GREEN_HIGHLIGHT = 80,
    PALETTE_TRANSLUCENT_OLIVE_GREEN_SHADOW = 81,
    PALETTE_TRANSLUCENT_BRIGHT_GREEN = 82,
    PALETTE_TRANSLUCENT_BRIGHT_GREEN_HIGHLIGHT = 83,
    PALETTE_TRANSLUCENT_BRIGHT_GREEN_SHADOW = 84,
    PALETTE_TRANSLUCENT_SALMON_PINK = 85,
    PALETTE_TRANSLUCENT_SALMON_PINK_HIGHLIGHT = 86,
    PALETTE_TRANSLUCENT_SALMON_PINK_SHADOW = 87,
    PALETTE_TRANSLUCENT_BRIGHT_PURPLE = 88,
    PALETTE_TRANSLUCENT_BRIGHT_PURPLE_HIGHLIGHT = 89,
    PALETTE_TRANSLUCENT_BRIGHT_PURPLE_SHADOW = 90,
    PALETTE_TRANSLUCENT_BRIGHT_RED = 91,
    PALETTE_TRANSLUCENT_BRIGHT_RED_HIGHLIGHT = 92,
    PALETTE_TRANSLUCENT_BRIGHT_RED_SHADOW = 93,
    PALETTE_TRANSLUCENT_LIGHT_ORANGE = 94,
    PALETTE_TRANSLUCENT_LIGHT_ORANGE_HIGHLIGHT = 95,
    PALETTE_TRANSLUCENT_LIGHT_ORANGE_SHADOW = 96,
    PALETTE_TRANSLUCENT_TEAL = 97,
    PALETTE_TRANSLUCENT_TEAL_HIGHLIGHT = 98,
    PALETTE_TRANSLUCENT_TEAL_SHADOW = 99,
    PALETTE_TRANSLUCENT_BRIGHT_PINK = 100,
    PALETTE_TRANSLUCENT_BRIGHT_PINK_HIGHLIGHT = 101,
    PALETTE_TRANSLUCENT_BRIGHT_PINK_SHADOW = 102,
    PALETTE_TRANSLUCENT_DARK_BROWN = 103,
    PALETTE_TRANSLUCENT_DARK_BROWN_HIGHLIGHT = 104,
    PALETTE_TRANSLUCENT_DARK_BROWN_SHADOW = 105,
    PALETTE_TRANSLUCENT_LIGHT_PINK = 106,
    PALETTE_TRANSLUCENT_LIGHT_PINK_HIGHLIGHT = 107,
    PALETTE_TRANSLUCENT_LIGHT_PINK_SHADOW = 108,
    PALETTE_TRANSLUCENT_WHITE = 109,
    PALETTE_TRANSLUCENT_WHITE_HIGHLIGHT = 110,
    PALETTE_TRANSLUCENT_WHITE_SHADOW = 111,
    PALETTE_GLASS_BLACK = 112 + COLOUR_BLACK,
    PALETTE_GLASS_GREY = 112 + COLOUR_GREY,
    PALETTE_GLASS_WHITE = 112 + COLOUR_WHITE,
    PALETTE_GLASS_DARK_PURPLE = 112 + COLOUR_DARK_PURPLE,
    PALETTE_GLASS_LIGHT_PURPLE = 112 + COLOUR_LIGHT_PURPLE,
    PALETTE_GLASS_BRIGHT_PURPLE = 112 + COLOUR_BRIGHT_PURPLE,
    PALETTE_GLASS_DARK_BLUE = 112 + COLOUR_DARK_BLUE,
    PALETTE_GLASS_LIGHT_BLUE = 112 + COLOUR_LIGHT_BLUE,
    PALETTE_GLASS_ICY_BLUE = 112 + COLOUR_ICY_BLUE,
    PALETTE_GLASS_TEAL = 112 + COLOUR_TEAL,
    PALETTE_GLASS_AQUAMARINE = 112 + COLOUR_AQUAMARINE,
    PALETTE_GLASS_SATURATED_GREEN = 112 + COLOUR_SATURATED_GREEN,
    PALETTE_GLASS_DARK_GREEN = 112 + COLOUR_DARK_GREEN,
    PALETTE_GLASS_MOSS_GREEN = 112 + COLOUR_MOSS_GREEN,
    PALETTE_GLASS_BRIGHT_GREEN = 112 + COLOUR_BRIGHT_GREEN,
    PALETTE_GLASS_OLIVE_GREEN = 112 + COLOUR_OLIVE_GREEN,
    PALETTE_GLASS_DARK_OLIVE_GREEN = 112 + COLOUR_DARK_OLIVE_GREEN,
    PALETTE_GLASS_BRIGHT_YELLOW = 112 + COLOUR_BRIGHT_YELLOW,
    PALETTE_GLASS_YELLOW = 112 + COLOUR_YELLOW,
    PALETTE_GLASS_DARK_YELLOW = 112 + COLOUR_DARK_YELLOW,
    PALETTE_GLASS_LIGHT_ORANGE = 112 + COLOUR_LIGHT_ORANGE,
    PALETTE_GLASS_DARK_ORANGE = 112 + COLOUR_DARK_ORANGE,
    PALETTE_GLASS_LIGHT_BROWN = 112 + COLOUR_LIGHT_BROWN,
    PALETTE_GLASS_SATURATED_BROWN = 112 + COLOUR_SATURATED_BROWN,
    PALETTE_GLASS_DARK_BROWN = 112 + COLOUR_DARK_BROWN,
    PALETTE_GLASS_SALMON_PINK = 112 + COLOUR_SALMON_PINK,
    PALETTE_GLASS_BORDEAUX_RED = 112 + COLOUR_BORDEAUX_RED,
    PALETTE_GLASS_SATURATED_RED = 112 + COLOUR_SATURATED_RED,
    PALETTE_GLASS_BRIGHT_RED = 112 + COLOUR_BRIGHT_RED,
    PALETTE_GLASS_DARK_PINK = 112 + COLOUR_DARK_PINK,
    PALETTE_GLASS_BRIGHT_PINK = 112 + COLOUR_BRIGHT_PINK,
    PALETTE_GLASS_LIGHT_PINK = 112 + COLOUR_LIGHT_PINK,
};

struct translucent_window_palette
{
    FILTER_PALETTE_ID base;
    FILTER_PALETTE_ID highlight;
    FILTER_PALETTE_ID shadow;
};

struct rct_size16
{
    int16_t width;
    int16_t height;
};

enum class ImageCatalogue
{
    UNKNOWN,
    G1,
    G2,
    CSG,
    OBJECT,
    TEMPORARY,
};

/**
 * Represents a specific image from a catalogue such as G1, G2, CSG etc. with remap
 * colours and flags.
 *
 * This is currently all stored as a single 32-bit integer, but will allow easy
 * extension to 64-bits or higher so that more images can be used.
 */
struct ImageId
{
private:
    // clang-format off
    static constexpr uint32_t MASK_INDEX       = 0b00000000000001111111111111111111;
    static constexpr uint32_t MASK_REMAP       = 0b00000111111110000000000000000000;
    static constexpr uint32_t MASK_PRIMARY     = 0b00000000111110000000000000000000;
    static constexpr uint32_t MASK_SECONDARY   = 0b00011111000000000000000000000000;
    static constexpr uint32_t FLAG_PRIMARY     = 0b00100000000000000000000000000000;
    static constexpr uint32_t FLAG_BLEND       = 0b01000000000000000000000000000000;
    static constexpr uint32_t FLAG_SECONDARY   = 0b10000000000000000000000000000000;
    static constexpr uint32_t SHIFT_REMAP      = 19;
    static constexpr uint32_t SHIFT_PRIMARY    = 19;
    static constexpr uint32_t SHIFT_SECONDARY  = 24;
    static constexpr uint32_t INDEX_UNDEFINED  = 0b00000000000001111111111111111111;
    static constexpr uint32_t VALUE_UNDEFINED  = INDEX_UNDEFINED;
    // clang-format on

    uint32_t _value = VALUE_UNDEFINED;
    uint8_t _tertiary = 0;

public:
    static ImageId FromUInt32(uint32_t value)
    {
        ImageId result;
        result._value = value;
        return result;
    }

    static ImageId FromUInt32(uint32_t value, uint32_t tertiary)
    {
        ImageId result;
        result._value = value;
        result._tertiary = tertiary & 0xFF;
        return result;
    }

    ImageId() = default;

    explicit constexpr ImageId(uint32_t index)
        : _value(index & MASK_INDEX)
    {
    }

    constexpr ImageId(uint32_t index, uint8_t primaryColourOrPalette)
        : ImageId(ImageId(index).WithPrimary(primaryColourOrPalette))
    {
    }

    constexpr ImageId(uint32_t index, colour_t primaryColour, colour_t secondaryColour)
        : ImageId(ImageId(index).WithPrimary(primaryColour).WithSecondary(secondaryColour))
    {
    }

    constexpr ImageId(uint32_t index, colour_t primaryColour, colour_t secondaryColour, colour_t tertiaryColour)
        : ImageId(ImageId(index).WithPrimary(primaryColour).WithSecondary(secondaryColour).WithTertiary(tertiaryColour))
    {
    }

    uint32_t ToUInt32() const
    {
        return _value;
    }

    bool HasValue() const
    {
        return GetIndex() != INDEX_UNDEFINED;
    }

    bool HasPrimary() const
    {
        return (_value & FLAG_PRIMARY) || (_value & FLAG_SECONDARY);
    }

    bool HasSecondary() const
    {
        return _value & FLAG_SECONDARY;
    }

    bool HasTertiary() const
    {
        return !(_value & FLAG_PRIMARY) && (_value & FLAG_SECONDARY);
    }

    bool IsRemap() const
    {
        return (_value & FLAG_PRIMARY) && !(_value & FLAG_SECONDARY);
    }

    bool IsBlended() const
    {
        return _value & FLAG_BLEND;
    }

    uint32_t GetIndex() const
    {
        return _value & MASK_INDEX;
    }

    uint8_t GetRemap() const
    {
        return (_value & MASK_REMAP) >> SHIFT_REMAP;
    }

    colour_t GetPrimary() const
    {
        return (_value & MASK_PRIMARY) >> SHIFT_PRIMARY;
    }

    colour_t GetSecondary() const
    {
        return (_value & MASK_SECONDARY) >> SHIFT_SECONDARY;
    }

    colour_t GetTertiary() const
    {
        return _tertiary;
    }

    ImageCatalogue GetCatalogue() const;

    constexpr ImageId WithIndex(uint32_t index)
    {
        ImageId result = *this;
        result._value = (_value & ~MASK_INDEX) | (index & MASK_INDEX);
        return result;
    }

    constexpr ImageId WithPrimary(colour_t colour)
    {
        ImageId result = *this;
        result._value = (_value & ~MASK_PRIMARY) | ((colour << SHIFT_PRIMARY) & MASK_PRIMARY) | FLAG_PRIMARY;
        return result;
    }

    constexpr ImageId WithSecondary(colour_t colour)
    {
        ImageId result = *this;
        result._value = (_value & ~MASK_SECONDARY) | ((colour << SHIFT_SECONDARY) & MASK_SECONDARY) | FLAG_SECONDARY;
        return result;
    }

    constexpr ImageId WithTertiary(colour_t tertiary)
    {
        ImageId result = *this;
        result._value &= ~FLAG_PRIMARY;
        if (!(_value & FLAG_SECONDARY))
        {
            // Tertiary implies primary and secondary, so if colour was remap (8-bit primary) then
            // we need to zero the secondary colour.
            result._value &= ~MASK_SECONDARY;
            result._value |= FLAG_SECONDARY;
        }
        result._tertiary = tertiary;
        return result;
    }
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
    rct_drawpixelinfo* DPI;
    ImageId Image;
    const PaletteMap& PalMap;
    const rct_g1_element& SourceImage;
    int32_t SrcX;
    int32_t SrcY;
    int32_t Width;
    int32_t Height;
    uint8_t* DestinationBits;

    DrawSpriteArgs(
        rct_drawpixelinfo* dpi, ImageId image, const PaletteMap& palMap, const rct_g1_element& sourceImage, int32_t srcX,
        int32_t srcY, int32_t width, int32_t height, uint8_t* destinationBits)
        : DPI(dpi)
        , Image(image)
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
#define SPRITE_ID_PALETTE_COLOUR_2(primaryId, secondaryId)                                                                     \
    (IMAGE_TYPE_REMAP_2_PLUS | IMAGE_TYPE_REMAP | (((primaryId) << 19) | ((secondaryId) << 24)))
#define SPRITE_ID_PALETTE_COLOUR_3(primaryId, secondaryId)                                                                     \
    (IMAGE_TYPE_REMAP_2_PLUS | (((primaryId) << 19) | ((secondaryId) << 24)))

#define PALETTE_TO_G1_OFFSET_COUNT 144

#define INSET_RECT_F_30 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE)
#define INSET_RECT_F_60 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN)
#define INSET_RECT_F_E0 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN | INSET_RECT_FLAG_FILL_MID_LIGHT)

#define MAX_SCROLLING_TEXT_MODES 38

extern thread_local int16_t gCurrentFontSpriteBase;
extern thread_local uint16_t gCurrentFontFlags;

extern GamePalette gPalette;
extern uint8_t gGamePalette[256 * 4];
extern uint32_t gPaletteEffectFrame;
extern const FILTER_PALETTE_ID GlassPaletteIds[COLOUR_COUNT];
extern uint8_t gPeepPalette[256];
extern uint8_t gOtherPalette[256];
extern uint8_t text_palette[];
extern const translucent_window_palette TranslucentWindowPalettes[COLOUR_COUNT];

extern thread_local int32_t gLastDrawStringX;
extern thread_local int32_t gLastDrawStringY;

extern uint32_t gPickupPeepImage;
extern int32_t gPickupPeepX;
extern int32_t gPickupPeepY;

extern bool gTinyFontAntiAliased;

extern rct_drawpixelinfo gScreenDPI;
extern rct_drawpixelinfo gWindowDPI;

bool clip_drawpixelinfo(
    rct_drawpixelinfo* dst, rct_drawpixelinfo* src, const ScreenCoordsXY& coords, int32_t width, int32_t height);
void gfx_set_dirty_blocks(const ScreenRect& rect);
void gfx_draw_all_dirty_blocks();
void gfx_invalidate_screen();

// palette
void gfx_transpose_palette(int32_t pal, uint8_t product);
void load_palette();

// other
void gfx_clear(rct_drawpixelinfo* dpi, uint8_t paletteIndex);
void gfx_draw_pixel(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t colour);
void gfx_filter_pixel(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, FILTER_PALETTE_ID palette);
void gfx_invalidate_pickedup_peep();
void gfx_draw_pickedup_peep(rct_drawpixelinfo* dpi);

// line
void gfx_draw_line(rct_drawpixelinfo* dpi, const ScreenLine& line, int32_t colour);
void gfx_draw_line_software(rct_drawpixelinfo* dpi, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t colour);
void gfx_draw_dashed_line(
    rct_drawpixelinfo* dpi, const ScreenLine& screenLine, const int32_t dashedLineSegmentLength, const int32_t color);

// rect
void gfx_fill_rect(rct_drawpixelinfo* dpi, const ScreenRect& rect, int32_t colour);
void gfx_fill_rect_inset(
    rct_drawpixelinfo* dpi, int16_t left, int16_t top, int16_t right, int16_t bottom, int32_t colour, uint8_t flags);
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, const ScreenRect& rect, int32_t colour, uint8_t flags);
void gfx_filter_rect(
    rct_drawpixelinfo* dpi, int32_t left, int32_t top, int32_t right, int32_t bottom, FILTER_PALETTE_ID palette);
void gfx_filter_rect(rct_drawpixelinfo* dpi, const ScreenRect& rect, FILTER_PALETTE_ID palette);

// sprite
bool gfx_load_g1(const OpenRCT2::IPlatformEnvironment& env);
bool gfx_load_g2();
bool gfx_load_csg();
void gfx_unload_g1();
void gfx_unload_g2();
void gfx_unload_csg();
const rct_g1_element* gfx_get_g1_element(ImageId imageId);
const rct_g1_element* gfx_get_g1_element(int32_t image_id);
void gfx_set_g1_element(int32_t imageId, const rct_g1_element* g1);
bool is_csg_loaded();
uint32_t gfx_object_allocate_images(const rct_g1_element* images, uint32_t count);
void gfx_object_free_images(uint32_t baseImageId, uint32_t count);
void gfx_object_check_all_images_freed();
size_t ImageListGetUsedCount();
size_t ImageListGetMaximum();
void FASTCALL gfx_sprite_to_buffer(DrawSpriteArgs& args);
void FASTCALL gfx_bmp_sprite_to_buffer(DrawSpriteArgs& args);
void FASTCALL gfx_rle_sprite_to_buffer(DrawSpriteArgs& args);
void FASTCALL gfx_draw_sprite(rct_drawpixelinfo* dpi, int32_t image_id, const ScreenCoordsXY& coords, uint32_t tertiary_colour);
void FASTCALL
    gfx_draw_glyph(rct_drawpixelinfo* dpi, int32_t image_id, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL
    gfx_draw_sprite_raw_masked(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t maskImage, int32_t colourImage);
void FASTCALL gfx_draw_sprite_solid(rct_drawpixelinfo* dpi, int32_t image, const ScreenCoordsXY& coords, uint8_t colour);

void FASTCALL gfx_draw_sprite_software(rct_drawpixelinfo* dpi, ImageId imageId, const ScreenCoordsXY& spriteCoords);
void FASTCALL gfx_draw_sprite_palette_set_software(
    rct_drawpixelinfo* dpi, ImageId imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap);
void FASTCALL gfx_draw_sprite_raw_masked_software(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& scrCoords, int32_t maskImage, int32_t colourImage);

// string
void gfx_draw_string(rct_drawpixelinfo* dpi, const_utf8string buffer, uint8_t colour, const ScreenCoordsXY& coords);

/** @deprecated */
void gfx_draw_string_left(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords);
/** @deprecated */
void gfx_draw_string_centred(
    rct_drawpixelinfo* dpi, rct_string_id format, const ScreenCoordsXY& coords, uint8_t colour, const void* args);
/** @deprecated */
void gfx_draw_string_right(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords);

/** @deprecated */
void gfx_draw_string_left_clipped(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords, int32_t width);
/** @deprecated */
void gfx_draw_string_centred_clipped(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords, int32_t width);
/** @deprecated */
void gfx_draw_string_right_clipped(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords, int32_t width);

int32_t gfx_draw_string_left_wrapped(
    rct_drawpixelinfo* dpi, void* args, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, uint8_t colour);
int32_t gfx_draw_string_centred_wrapped(
    rct_drawpixelinfo* dpi, void* args, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, uint8_t colour);

void gfx_draw_string_left_centred(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, int32_t colour, const ScreenCoordsXY& coords);
void draw_string_centred_raw(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t numLines, char* text);
void gfx_draw_string_centred_wrapped_partial(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, int32_t colour, rct_string_id format, void* args,
    int32_t ticks);
void gfx_draw_string_with_y_offsets(
    rct_drawpixelinfo* dpi, const utf8* text, int32_t colour, const ScreenCoordsXY& coords, const int8_t* yOffsets,
    bool forceSpriteFont);

int32_t gfx_wrap_string(char* buffer, int32_t width, int32_t* num_lines, int32_t* font_height);
int32_t gfx_get_string_width(const utf8* buffer);
int32_t gfx_get_string_width_new_lined(char* buffer);
int32_t string_get_height_raw(char* buffer);
int32_t gfx_clip_string(char* buffer, int32_t width);
void shorten_path(utf8* buffer, size_t bufferSize, const utf8* path, int32_t availableWidth);
void ttf_draw_string(rct_drawpixelinfo* dpi, const_utf8string text, int32_t colour, const ScreenCoordsXY& coords);

// scrolling text
void scrolling_text_initialise_bitmaps();
void scrolling_text_invalidate();

class Formatter;

int32_t scrolling_text_setup(
    struct paint_session* session, rct_string_id stringId, Formatter& ft, uint16_t scroll, uint16_t scrollingMode,
    colour_t colour);

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

#include "NewDrawing.h"

#endif
