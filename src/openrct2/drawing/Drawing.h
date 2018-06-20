/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

struct rct_g1_element {
    uint8_t* offset;          // 0x00
    int16_t width;           // 0x04
    int16_t height;          // 0x06
    int16_t x_offset;        // 0x08
    int16_t y_offset;        // 0x0A
    uint16_t flags;           // 0x0C
    uint16_t zoomed_offset;   // 0x0E
};

struct rct_drawpixelinfo {
    uint8_t* bits;        // 0x00
    int16_t x;           // 0x04
    int16_t y;           // 0x06
    int16_t width;       // 0x08
    int16_t height;      // 0x0A
    int16_t pitch;       // 0x0C         note: this is actually (pitch - width)
    uint16_t zoom_level;  // 0x0E
};

struct rct_g1_element_32bit {
    uint32_t offset;                  // 0x00 note: uint32_t always!
    int16_t width;                   // 0x04
    int16_t height;                  // 0x06
    int16_t x_offset;                // 0x08
    int16_t y_offset;                // 0x0A
    uint16_t flags;                   // 0x0C
    uint16_t zoomed_offset;           // 0x0E
};
assert_struct_size(rct_g1_element_32bit, 0x10);


enum {
    G1_FLAG_BMP             = (1 << 0), // Image data is encoded as raw pixels (no transparency)
    G1_FLAG_1               = (1 << 1),
    G1_FLAG_RLE_COMPRESSION = (1 << 2), // Image data is encoded using RCT2's form of run length encoding
    G1_FLAG_PALETTE         = (1 << 3), // Image data is a sequence of palette entries R8G8B8
    G1_FLAG_HAS_ZOOM_SPRITE = (1 << 4), // Use a different sprite for higher zoom levels
    G1_FLAG_NO_ZOOM_DRAW    = (1 << 5), // Does not get drawn at higher zoom levels (only zoom 0)
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

enum {
    INSET_RECT_FLAG_FILL_GREY = (1 << 2), // 0x04
    INSET_RECT_FLAG_BORDER_NONE = (1 << 3), // 0x08
    INSET_RECT_FLAG_FILL_NONE = (1 << 4), // 0x10
    INSET_RECT_FLAG_BORDER_INSET = (1 << 5), // 0x20
    INSET_RECT_FLAG_FILL_DONT_LIGHTEN = (1 << 6), // 0x40
    INSET_RECT_FLAG_FILL_MID_LIGHT = (1 << 7), // 0x80
};

enum FILTER_PALETTE_ID {
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
    PALETTE_GLASS_BLACK             = 112 + COLOUR_BLACK,
    PALETTE_GLASS_GREY              = 112 + COLOUR_GREY,
    PALETTE_GLASS_WHITE             = 112 + COLOUR_WHITE,
    PALETTE_GLASS_DARK_PURPLE       = 112 + COLOUR_DARK_PURPLE,
    PALETTE_GLASS_LIGHT_PURPLE      = 112 + COLOUR_LIGHT_PURPLE,
    PALETTE_GLASS_BRIGHT_PURPLE     = 112 + COLOUR_BRIGHT_PURPLE,
    PALETTE_GLASS_DARK_BLUE         = 112 + COLOUR_DARK_BLUE,
    PALETTE_GLASS_LIGHT_BLUE        = 112 + COLOUR_LIGHT_BLUE,
    PALETTE_GLASS_ICY_BLUE          = 112 + COLOUR_ICY_BLUE,
    PALETTE_GLASS_TEAL              = 112 + COLOUR_TEAL,
    PALETTE_GLASS_AQUAMARINE        = 112 + COLOUR_AQUAMARINE,
    PALETTE_GLASS_SATURATED_GREEN   = 112 + COLOUR_SATURATED_GREEN,
    PALETTE_GLASS_DARK_GREEN        = 112 + COLOUR_DARK_GREEN,
    PALETTE_GLASS_MOSS_GREEN        = 112 + COLOUR_MOSS_GREEN,
    PALETTE_GLASS_BRIGHT_GREEN      = 112 + COLOUR_BRIGHT_GREEN,
    PALETTE_GLASS_OLIVE_GREEN       = 112 + COLOUR_OLIVE_GREEN,
    PALETTE_GLASS_DARK_OLIVE_GREEN  = 112 + COLOUR_DARK_OLIVE_GREEN,
    PALETTE_GLASS_BRIGHT_YELLOW     = 112 + COLOUR_BRIGHT_YELLOW,
    PALETTE_GLASS_YELLOW            = 112 + COLOUR_YELLOW,
    PALETTE_GLASS_DARK_YELLOW       = 112 + COLOUR_DARK_YELLOW,
    PALETTE_GLASS_LIGHT_ORANGE      = 112 + COLOUR_LIGHT_ORANGE,
    PALETTE_GLASS_DARK_ORANGE       = 112 + COLOUR_DARK_ORANGE,
    PALETTE_GLASS_LIGHT_BROWN       = 112 + COLOUR_LIGHT_BROWN,
    PALETTE_GLASS_SATURATED_BROWN   = 112 + COLOUR_SATURATED_BROWN,
    PALETTE_GLASS_DARK_BROWN        = 112 + COLOUR_DARK_BROWN,
    PALETTE_GLASS_SALMON_PINK       = 112 + COLOUR_SALMON_PINK,
    PALETTE_GLASS_BORDEAUX_RED      = 112 + COLOUR_BORDEAUX_RED,
    PALETTE_GLASS_SATURATED_RED     = 112 + COLOUR_SATURATED_RED,
    PALETTE_GLASS_BRIGHT_RED        = 112 + COLOUR_BRIGHT_RED,
    PALETTE_GLASS_DARK_PINK         = 112 + COLOUR_DARK_PINK,
    PALETTE_GLASS_BRIGHT_PINK       = 112 + COLOUR_BRIGHT_PINK,
    PALETTE_GLASS_LIGHT_PINK        = 112 + COLOUR_LIGHT_PINK,
};

struct translucent_window_palette {
    FILTER_PALETTE_ID base;
    FILTER_PALETTE_ID highlight;
    FILTER_PALETTE_ID shadow;
};

#pragma pack(push, 1)

struct rct_palette_entry {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
};
assert_struct_size(rct_palette_entry, 4);

#pragma pack(pop)

struct rct_palette {
    rct_palette_entry entries[256];
};

struct rct_size16
{
    int16_t width;
    int16_t height;
};

#define SPRITE_ID_PALETTE_COLOUR_1(colourId) (IMAGE_TYPE_REMAP | ((colourId) << 19))
#define SPRITE_ID_PALETTE_COLOUR_2(primaryId, secondaryId) (IMAGE_TYPE_REMAP_2_PLUS | IMAGE_TYPE_REMAP | (((primaryId) << 19) | ((secondaryId) << 24)))
#define SPRITE_ID_PALETTE_COLOUR_3(primaryId, secondaryId) (IMAGE_TYPE_REMAP_2_PLUS | (((primaryId) << 19) | ((secondaryId) << 24)))

#define PALETTE_TO_G1_OFFSET_COUNT 144

#define INSET_RECT_F_30 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE)
#define INSET_RECT_F_60 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN)
#define INSET_RECT_F_E0 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN | INSET_RECT_FLAG_FILL_MID_LIGHT)

#define MAX_SCROLLING_TEXT_MODES 38

extern int16_t gCurrentFontSpriteBase;
extern uint16_t gCurrentFontFlags;

extern rct_palette_entry gPalette[256];
extern uint8_t gGamePalette[256 * 4];
extern uint32_t gPaletteEffectFrame;
extern const FILTER_PALETTE_ID GlassPaletteIds[COLOUR_COUNT];
extern const uint16_t palette_to_g1_offset[];
extern uint8_t gPeepPalette[256];
extern uint8_t gOtherPalette[256];
extern uint8_t text_palette[];
extern const translucent_window_palette TranslucentWindowPalettes[COLOUR_COUNT];

extern int32_t gLastDrawStringX;
extern int32_t gLastDrawStringY;

extern uint32_t gPickupPeepImage;
extern int32_t gPickupPeepX;
extern int32_t gPickupPeepY;

extern bool gTinyFontAntiAliased;

extern rct_drawpixelinfo gScreenDPI;
extern rct_drawpixelinfo gWindowDPI;

//
bool clip_drawpixelinfo(rct_drawpixelinfo *dst, rct_drawpixelinfo *src, int32_t x, int32_t y, int32_t width, int32_t height);
void gfx_set_dirty_blocks(int16_t left, int16_t top, int16_t right, int16_t bottom);
void gfx_draw_all_dirty_blocks();
void gfx_invalidate_screen();

// palette
void gfx_transpose_palette(int32_t pal, uint8_t product);
void load_palette();

// other
void gfx_clear(rct_drawpixelinfo *dpi, uint8_t paletteIndex);
void gfx_draw_pixel(rct_drawpixelinfo *dpi, int32_t x, int32_t y, int32_t colour);
void gfx_filter_pixel(rct_drawpixelinfo *dpi, int32_t x, int32_t y, FILTER_PALETTE_ID palette);
void gfx_invalidate_pickedup_peep();
void gfx_draw_pickedup_peep(rct_drawpixelinfo *dpi);

// line
void gfx_draw_line(rct_drawpixelinfo *dpi, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t colour);
void gfx_draw_line_software(rct_drawpixelinfo *dpi, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t colour);

// rect
void gfx_fill_rect(rct_drawpixelinfo *dpi, int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t colour);
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, int16_t left, int16_t top, int16_t right, int16_t bottom, int32_t colour, uint8_t flags);
void gfx_filter_rect(rct_drawpixelinfo *dpi, int32_t left, int32_t top, int32_t right, int32_t bottom, FILTER_PALETTE_ID palette);

// sprite
bool gfx_load_g1(const OpenRCT2::IPlatformEnvironment& env);
bool gfx_load_g2();
bool gfx_load_csg();
void gfx_unload_g1();
void gfx_unload_g2();
void gfx_unload_csg();
const rct_g1_element * gfx_get_g1_element(int32_t image_id);
void gfx_set_g1_element(int32_t imageId, const rct_g1_element * g1);
bool is_csg_loaded();
uint32_t gfx_object_allocate_images(const rct_g1_element * images, uint32_t count);
void gfx_object_free_images(uint32_t baseImageId, uint32_t count);
void gfx_object_check_all_images_freed();
void FASTCALL gfx_bmp_sprite_to_buffer(const uint8_t* palette_pointer, uint8_t* source_pointer, uint8_t* dest_pointer, const rct_g1_element* source_image, rct_drawpixelinfo *dest_dpi, int32_t height, int32_t width, int32_t image_type);
void FASTCALL gfx_rle_sprite_to_buffer(const uint8_t* RESTRICT source_bits_pointer, uint8_t* RESTRICT dest_bits_pointer, const uint8_t* RESTRICT palette_pointer, const rct_drawpixelinfo * RESTRICT dpi, int32_t image_type, int32_t source_y_start, int32_t height, int32_t source_x_start, int32_t width);
void FASTCALL gfx_draw_sprite(rct_drawpixelinfo *dpi, int32_t image_id, int32_t x, int32_t y, uint32_t tertiary_colour);
void FASTCALL gfx_draw_glpyh(rct_drawpixelinfo *dpi, int32_t image_id, int32_t x, int32_t y, uint8_t * palette);
void FASTCALL gfx_draw_sprite_raw_masked(rct_drawpixelinfo *dpi, int32_t x, int32_t y, int32_t maskImage, int32_t colourImage);
void FASTCALL gfx_draw_sprite_solid(rct_drawpixelinfo * dpi, int32_t image, int32_t x, int32_t y, uint8_t colour);

void FASTCALL gfx_draw_sprite_software(rct_drawpixelinfo *dpi, int32_t image_id, int32_t x, int32_t y, uint32_t tertiary_colour);
uint8_t* FASTCALL gfx_draw_sprite_get_palette(int32_t image_id, uint32_t tertiary_colour);
void FASTCALL gfx_draw_sprite_palette_set_software(rct_drawpixelinfo *dpi, int32_t image_id, int32_t x, int32_t y, uint8_t* palette_pointer, uint8_t* unknown_pointer);
void FASTCALL gfx_draw_sprite_raw_masked_software(rct_drawpixelinfo *dpi, int32_t x, int32_t y, int32_t maskImage, int32_t colourImage);

// string
void gfx_draw_string(rct_drawpixelinfo *dpi, const_utf8string buffer, uint8_t colour, int32_t x, int32_t y);

void gfx_draw_string_left(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8_t colour, int32_t x, int32_t y);
void gfx_draw_string_centred(rct_drawpixelinfo *dpi, rct_string_id format, int32_t x, int32_t y, uint8_t colour, void *args);
void gfx_draw_string_right(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8_t colour, int32_t x, int32_t y);

void draw_string_left_underline(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8_t colour, int32_t x, int32_t y);
void draw_string_centred_underline(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8_t colour, int32_t x, int32_t y);
void draw_string_right_underline(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8_t colour, int32_t x, int32_t y);

void gfx_draw_string_left_clipped(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8_t colour, int32_t x, int32_t y, int32_t width);
void gfx_draw_string_centred_clipped(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8_t colour, int32_t x, int32_t y, int32_t width);
void gfx_draw_string_right_clipped(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8_t colour, int32_t x, int32_t y, int32_t width);

int32_t gfx_draw_string_left_wrapped(rct_drawpixelinfo *dpi, void *args, int32_t x, int32_t y, int32_t width, rct_string_id format, uint8_t colour);
int32_t gfx_draw_string_centred_wrapped(rct_drawpixelinfo *dpi, void *args, int32_t x, int32_t y, int32_t width, rct_string_id format, uint8_t colour);

void gfx_draw_string_left_centred(rct_drawpixelinfo *dpi, rct_string_id format, void *args, int32_t colour, int32_t x, int32_t y);
void draw_string_centred_raw(rct_drawpixelinfo *dpi, int32_t x, int32_t y, int32_t numLines, char *text);
void gfx_draw_string_centred_wrapped_partial(rct_drawpixelinfo *dpi, int32_t x, int32_t y, int32_t width, int32_t colour, rct_string_id format, void *args, int32_t ticks);
void gfx_draw_string_with_y_offsets(rct_drawpixelinfo *dpi, const utf8 *text, int32_t colour, int32_t x, int32_t y, const int8_t *yOffsets, bool forceSpriteFont);

int32_t gfx_wrap_string(char* buffer, int32_t width, int32_t* num_lines, int32_t* font_height);
int32_t gfx_get_string_width(const utf8 * buffer);
int32_t gfx_get_string_width_new_lined(char* buffer);
int32_t string_get_height_raw(char *buffer);
int32_t gfx_clip_string(char* buffer, int32_t width);
void shorten_path(utf8 *buffer, size_t bufferSize, const utf8 *path, int32_t availableWidth);
void ttf_draw_string(rct_drawpixelinfo *dpi, const_utf8string text, int32_t colour, int32_t x, int32_t y);

// scrolling text
void scrolling_text_initialise_bitmaps();
int32_t scrolling_text_setup(struct paint_session * session, rct_string_id stringId, uint16_t scroll, uint16_t scrollingMode);

rct_size16 FASTCALL gfx_get_sprite_size(uint32_t image_id);
size_t g1_calculate_data_size(const rct_g1_element * g1);

void mask_scalar(int32_t width, int32_t height, const uint8_t * RESTRICT maskSrc, const uint8_t * RESTRICT colourSrc,
                 uint8_t * RESTRICT dst, int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
void mask_sse4_1(int32_t width, int32_t height, const uint8_t * RESTRICT maskSrc, const uint8_t * RESTRICT colourSrc,
                 uint8_t * RESTRICT dst, int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
void mask_avx2(int32_t width, int32_t height, const uint8_t * RESTRICT maskSrc, const uint8_t * RESTRICT colourSrc,
               uint8_t * RESTRICT dst, int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);
void mask_init();

extern void (*mask_fn)(int32_t width, int32_t height, const uint8_t * RESTRICT maskSrc, const uint8_t * RESTRICT colourSrc,
                       uint8_t * RESTRICT dst, int32_t maskWrap, int32_t colourWrap, int32_t dstWrap);

#include "NewDrawing.h"

#endif
