#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _DRAWING_H_
#define _DRAWING_H_

#include "../common.h"
#include "../interface/Colour.h"

struct rct_g1_element {
    uint8* offset;          // 0x00
    sint16 width;           // 0x04
    sint16 height;          // 0x06
    sint16 x_offset;        // 0x08
    sint16 y_offset;        // 0x0A
    uint16 flags;           // 0x0C
    uint16 zoomed_offset;   // 0x0E
};

struct rct_drawpixelinfo {
    uint8* bits;        // 0x00
    sint16 x;           // 0x04
    sint16 y;           // 0x06
    sint16 width;       // 0x08
    sint16 height;      // 0x0A
    sint16 pitch;       // 0x0C         note: this is actually (pitch - width)
    uint16 zoom_level;  // 0x0E
};

struct rct_g1_element_32bit {
    uint32 offset;                  // 0x00 note: uint32 always!
    sint16 width;                   // 0x04
    sint16 height;                  // 0x06
    sint16 x_offset;                // 0x08
    sint16 y_offset;                // 0x0A
    uint16 flags;                   // 0x0C
    uint16 zoomed_offset;           // 0x0E
};
assert_struct_size(rct_g1_element_32bit, 0x10);


enum {
    G1_FLAG_BMP             = (1 << 0), // Image data is encoded as raw pixels (no transparency)
    G1_FLAG_1               = (1 << 1),
    G1_FLAG_RLE_COMPRESSION = (1 << 2), // Image data is encoded using RCT2's form of run length encoding
    G1_FLAG_HAS_ZOOM_SPRITE = (1 << 4), // Use a different sprite for higher zoom levels
    G1_FLAG_NO_ZOOM_DRAW    = (1 << 5), // Does not get drawn at higher zoom levels (only zoom 0)
};

enum {
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
    uint8 blue;
    uint8 green;
    uint8 red;
    uint8 alpha;
};
assert_struct_size(rct_palette_entry, 4);

#pragma pack(pop)

struct rct_palette {
    rct_palette_entry entries[256];
};

struct rct_size16
{
    sint16 width;
    sint16 height;
};

#define SPRITE_ID_PALETTE_COLOUR_1(colourId) (IMAGE_TYPE_REMAP | ((colourId) << 19))
#define SPRITE_ID_PALETTE_COLOUR_2(primaryId, secondaryId) (IMAGE_TYPE_REMAP_2_PLUS | IMAGE_TYPE_REMAP | ((primaryId << 19) | (secondaryId << 24)))
#define SPRITE_ID_PALETTE_COLOUR_3(primaryId, secondaryId) (IMAGE_TYPE_REMAP_2_PLUS | ((primaryId << 19) | (secondaryId << 24)))

#define PALETTE_TO_G1_OFFSET_COUNT 144

#define INSET_RECT_F_30 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE)
#define INSET_RECT_F_60 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN)
#define INSET_RECT_F_E0 (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN | INSET_RECT_FLAG_FILL_MID_LIGHT)

#define MAX_SCROLLING_TEXT_MODES 38

extern sint16 gCurrentFontSpriteBase;
extern uint16 gCurrentFontFlags;

extern rct_palette_entry gPalette[256];
extern uint8 gGamePalette[256 * 4];
extern uint32 gPaletteEffectFrame;
extern const FILTER_PALETTE_ID GlassPaletteIds[COLOUR_COUNT];
extern const uint16 palette_to_g1_offset[];
extern uint8 gPeepPalette[256];
extern uint8 gOtherPalette[256];
extern uint8 text_palette[];
extern const translucent_window_palette TranslucentWindowPalettes[COLOUR_COUNT];

extern sint32 gLastDrawStringX;
extern sint32 gLastDrawStringY;

extern uint32 gPickupPeepImage;
extern sint32 gPickupPeepX;
extern sint32 gPickupPeepY;

extern bool gTinyFontAntiAliased;

extern rct_drawpixelinfo gScreenDPI;
extern rct_drawpixelinfo gWindowDPI;

//
bool clip_drawpixelinfo(rct_drawpixelinfo *dst, rct_drawpixelinfo *src, sint32 x, sint32 y, sint32 width, sint32 height);
void gfx_set_dirty_blocks(sint16 left, sint16 top, sint16 right, sint16 bottom);
void gfx_draw_all_dirty_blocks();
void gfx_invalidate_screen();

// palette
void gfx_transpose_palette(sint32 pal, uint8 product);
void load_palette();

// other
void gfx_clear(rct_drawpixelinfo *dpi, uint8 paletteIndex);
void gfx_draw_pixel(rct_drawpixelinfo *dpi, sint32 x, sint32 y, sint32 colour);
void gfx_filter_pixel(rct_drawpixelinfo *dpi, sint32 x, sint32 y, FILTER_PALETTE_ID palette);
void gfx_invalidate_pickedup_peep();
void gfx_draw_pickedup_peep(rct_drawpixelinfo *dpi);

// line
void gfx_draw_line(rct_drawpixelinfo *dpi, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 colour);
void gfx_draw_line_software(rct_drawpixelinfo *dpi, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 colour);

// rect
void gfx_fill_rect(rct_drawpixelinfo *dpi, sint32 left, sint32 top, sint32 right, sint32 bottom, sint32 colour);
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, sint16 left, sint16 top, sint16 right, sint16 bottom, sint32 colour, uint8 flags);
void gfx_filter_rect(rct_drawpixelinfo *dpi, sint32 left, sint32 top, sint32 right, sint32 bottom, FILTER_PALETTE_ID palette);

// sprite
bool gfx_load_g1(void * platformEnvironment);
bool gfx_load_g2();
bool gfx_load_csg();
void gfx_unload_g1();
void gfx_unload_g2();
void gfx_unload_csg();
const rct_g1_element * gfx_get_g1_element(sint32 image_id);
void gfx_set_g1_element(sint32 imageId, const rct_g1_element * g1);
bool is_csg_loaded();
uint32 gfx_object_allocate_images(const rct_g1_element * images, uint32 count);
void gfx_object_free_images(uint32 baseImageId, uint32 count);
void gfx_object_check_all_images_freed();
void FASTCALL gfx_bmp_sprite_to_buffer(const uint8* palette_pointer, uint8* unknown_pointer, uint8* source_pointer, uint8* dest_pointer, const rct_g1_element* source_image, rct_drawpixelinfo *dest_dpi, sint32 height, sint32 width, sint32 image_type);
void FASTCALL gfx_rle_sprite_to_buffer(const uint8* RESTRICT source_bits_pointer, uint8* RESTRICT dest_bits_pointer, const uint8* RESTRICT palette_pointer, const rct_drawpixelinfo * RESTRICT dpi, sint32 image_type, sint32 source_y_start, sint32 height, sint32 source_x_start, sint32 width);
void FASTCALL gfx_draw_sprite(rct_drawpixelinfo *dpi, sint32 image_id, sint32 x, sint32 y, uint32 tertiary_colour);
void FASTCALL gfx_draw_glpyh(rct_drawpixelinfo *dpi, sint32 image_id, sint32 x, sint32 y, uint8 * palette);
void FASTCALL gfx_draw_sprite_raw_masked(rct_drawpixelinfo *dpi, sint32 x, sint32 y, sint32 maskImage, sint32 colourImage);
void FASTCALL gfx_draw_sprite_solid(rct_drawpixelinfo * dpi, sint32 image, sint32 x, sint32 y, uint8 colour);

void FASTCALL gfx_draw_sprite_software(rct_drawpixelinfo *dpi, sint32 image_id, sint32 x, sint32 y, uint32 tertiary_colour);
uint8* FASTCALL gfx_draw_sprite_get_palette(sint32 image_id, uint32 tertiary_colour);
void FASTCALL gfx_draw_sprite_palette_set_software(rct_drawpixelinfo *dpi, sint32 image_id, sint32 x, sint32 y, uint8* palette_pointer, uint8* unknown_pointer);
void FASTCALL gfx_draw_sprite_raw_masked_software(rct_drawpixelinfo *dpi, sint32 x, sint32 y, sint32 maskImage, sint32 colourImage);

// string
void gfx_draw_string(rct_drawpixelinfo *dpi, const_utf8string buffer, uint8 colour, sint32 x, sint32 y);

void gfx_draw_string_left(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8 colour, sint32 x, sint32 y);
void gfx_draw_string_centred(rct_drawpixelinfo *dpi, rct_string_id format, sint32 x, sint32 y, uint8 colour, void *args);
void gfx_draw_string_right(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8 colour, sint32 x, sint32 y);

void draw_string_left_underline(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8 colour, sint32 x, sint32 y);
void draw_string_centred_underline(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8 colour, sint32 x, sint32 y);
void draw_string_right_underline(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8 colour, sint32 x, sint32 y);

void gfx_draw_string_left_clipped(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8 colour, sint32 x, sint32 y, sint32 width);
void gfx_draw_string_centred_clipped(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8 colour, sint32 x, sint32 y, sint32 width);
void gfx_draw_string_right_clipped(rct_drawpixelinfo *dpi, rct_string_id format, void *args, uint8 colour, sint32 x, sint32 y, sint32 width);

sint32 gfx_draw_string_left_wrapped(rct_drawpixelinfo *dpi, void *args, sint32 x, sint32 y, sint32 width, rct_string_id format, uint8 colour);
sint32 gfx_draw_string_centred_wrapped(rct_drawpixelinfo *dpi, void *args, sint32 x, sint32 y, sint32 width, rct_string_id format, uint8 colour);

void gfx_draw_string_left_centred(rct_drawpixelinfo *dpi, rct_string_id format, void *args, sint32 colour, sint32 x, sint32 y);
void draw_string_centred_raw(rct_drawpixelinfo *dpi, sint32 x, sint32 y, sint32 numLines, char *text);
void gfx_draw_string_centred_wrapped_partial(rct_drawpixelinfo *dpi, sint32 x, sint32 y, sint32 width, sint32 colour, rct_string_id format, void *args, sint32 ticks);
void gfx_draw_string_with_y_offsets(rct_drawpixelinfo *dpi, const utf8 *text, sint32 colour, sint32 x, sint32 y, const sint8 *yOffsets, bool forceSpriteFont);

sint32 gfx_wrap_string(char* buffer, sint32 width, sint32* num_lines, sint32* font_height);
sint32 gfx_get_string_width(const utf8 * buffer);
sint32 gfx_get_string_width_new_lined(char* buffer);
sint32 string_get_height_raw(char *buffer);
sint32 gfx_clip_string(char* buffer, sint32 width);
void shorten_path(utf8 *buffer, size_t bufferSize, const utf8 *path, sint32 availableWidth);
void ttf_draw_string(rct_drawpixelinfo *dpi, const_utf8string text, sint32 colour, sint32 x, sint32 y);

// scrolling text
void scrolling_text_initialise_bitmaps();
sint32 scrolling_text_setup(struct paint_session * session, rct_string_id stringId, uint16 scroll, uint16 scrollingMode);

rct_size16 FASTCALL gfx_get_sprite_size(uint32 image_id);
size_t g1_calculate_data_size(const rct_g1_element * g1);

void mask_scalar(sint32 width, sint32 height, const uint8 * RESTRICT maskSrc, const uint8 * RESTRICT colourSrc,
                 uint8 * RESTRICT dst, sint32 maskWrap, sint32 colourWrap, sint32 dstWrap);
void mask_sse4_1(sint32 width, sint32 height, const uint8 * RESTRICT maskSrc, const uint8 * RESTRICT colourSrc,
                 uint8 * RESTRICT dst, sint32 maskWrap, sint32 colourWrap, sint32 dstWrap);
void mask_avx2(sint32 width, sint32 height, const uint8 * RESTRICT maskSrc, const uint8 * RESTRICT colourSrc,
               uint8 * RESTRICT dst, sint32 maskWrap, sint32 colourWrap, sint32 dstWrap);
void mask_init();

extern void (*mask_fn)(sint32 width, sint32 height, const uint8 * RESTRICT maskSrc, const uint8 * RESTRICT colourSrc,
                       uint8 * RESTRICT dst, sint32 maskWrap, sint32 colourWrap, sint32 dstWrap);

#include "NewDrawing.h"

#endif
