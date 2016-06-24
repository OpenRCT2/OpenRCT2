#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include "font.h"

// For g1 only enable packing when still relying on vanilla
#ifndef NO_RCT2
#pragma pack(push, 1)
#endif
// Size: 0x10
typedef struct rct_g1_element {
	uint8* offset;			// 0x00
	sint16 width;			// 0x04
	sint16 height;			// 0x06
	sint16 x_offset;		// 0x08
	sint16 y_offset;		// 0x0A
	uint16 flags;			// 0x0C
	uint16 zoomed_offset;	// 0x0E
} rct_g1_element;
#ifndef NO_RCT2
#ifdef PLATFORM_32BIT
assert_struct_size(rct_g1_element, 0x10);
#endif
#pragma pack(pop)
#endif

// Enable packing for remaining elements
#pragma pack(push, 1)
// Size: 0x10
typedef struct rct_drawpixelinfo {
	uint8* bits;		// 0x00
	short x;			// 0x04
	short y;			// 0x06
	short width;		// 0x08
	short height;		// 0x0A
	short pitch;		// 0x0C			note: this is actually (pitch - width)
	uint16 zoom_level;	// 0x0E
} rct_drawpixelinfo;
#ifdef PLATFORM_32BIT
assert_struct_size(rct_drawpixelinfo, 0x10);
#endif

// Size: 0x10
typedef struct rct_g1_element_32bit {
	uint32 offset;                  // 0x00 note: uint32 always!
	sint16 width;                   // 0x04
	sint16 height;                  // 0x06
	sint16 x_offset;                // 0x08
	sint16 y_offset;                // 0x0A
	uint16 flags;                   // 0x0C
	uint16 zoomed_offset;   // 0x0E
} rct_g1_element_32bit;
assert_struct_size(rct_g1_element_32bit, 0x10);


enum {
	G1_FLAG_BMP = (1 << 0), //No invisible sections
	G1_FLAG_RLE_COMPRESSION = (1<<2),
};

enum {
	IMAGE_TYPE_NO_BACKGROUND = 0,
	IMAGE_TYPE_USE_PALETTE= (1 << 1),
	IMAGE_TYPE_MIX_BACKGROUND = (1<<2),
	IMAGE_TYPE_UNKNOWN = (1<<3)
};

typedef struct rct_g1_header {
	uint32 num_entries;
	uint32 total_size;
} rct_g1_header;
assert_struct_size(rct_g1_header, 8);

typedef struct rct_gx {
	rct_g1_header header;
	rct_g1_element *elements;
	void *data;
} rct_gx;
#ifdef PLATFORM_32BIT
assert_struct_size(rct_gx, 16);
#endif

typedef struct rct_palette_entry {
	uint8 blue;
	uint8 green;
	uint8 red;
	uint8 alpha;
} rct_palette_entry;
assert_struct_size(rct_palette_entry, 4);

#pragma pack(pop)

typedef struct rct_palette {
	rct_palette_entry entries[256];
} rct_palette;

#define SPRITE_ID_PALETTE_COLOUR_1(colourId) ((IMAGE_TYPE_USE_PALETTE << 28) | ((colourId) << 19))

#define PALETTE_TO_G1_OFFSET_COUNT 144

#define gCurrentFontSpriteBase		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16)
#define gCurrentFontFlags			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16)

extern uint8 gGamePalette[256 * 4];
extern uint32 gPaletteEffectFrame;
extern const uint16 palette_to_g1_offset[];
extern const uint8 peep_palette[];
extern uint8 text_palette[];

extern int gLastDrawStringX;
extern int gLastDrawStringY;

extern uint32 gPickupPeepImage;
extern sint32 gPickupPeepX;
extern sint32 gPickupPeepY;

extern rct_g1_element *g1Elements;
extern rct_gx g2;

extern rct_drawpixelinfo gScreenDPI;
extern rct_drawpixelinfo gWindowDPI;

extern void *unk_9ABDA4;
extern void *unk_9E3CDC;
extern void *unk_9E3CE4[8];
#if NO_RCT2
extern rct_drawpixelinfo *unk_140E9A8;
#else
#define unk_140E9A8 RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*)
#endif

//
bool clip_drawpixelinfo(rct_drawpixelinfo *dst, rct_drawpixelinfo *src, int x, int y, int width, int height);
void gfx_set_dirty_blocks(sint16 left, sint16 top, sint16 right, sint16 bottom);
void gfx_draw_all_dirty_blocks();
void gfx_invalidate_screen();

// palette
void gfx_transpose_palette(int pal, unsigned char product);
void load_palette();

// other
void gfx_clear(rct_drawpixelinfo *dpi, int colour);
void gfx_draw_pixel(rct_drawpixelinfo *dpi, int x, int y, int colour);
void gfx_invalidate_pickedup_peep();
void gfx_draw_pickedup_peep(rct_drawpixelinfo *dpi);

// line
void gfx_draw_line(rct_drawpixelinfo *dpi, int x1, int y1, int x2, int y2, int colour);
void gfx_draw_line_software(rct_drawpixelinfo *dpi, int x1, int y1, int x2, int y2, int colour);

// rect
void gfx_fill_rect(rct_drawpixelinfo *dpi, int left, int top, int right, int bottom, int colour);
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, short left, short top, short right, short bottom, int colour, short _si);

// sprite
int gfx_load_g1();
int gfx_load_g2();
void gfx_unload_g1();
void gfx_unload_g2();
rct_g1_element* gfx_get_g1_element(int image_id);
void sub_68371D();
void FASTCALL gfx_rle_sprite_to_buffer(const uint8* RESTRICT source_bits_pointer, uint8* RESTRICT dest_bits_pointer, const uint8* RESTRICT palette_pointer, const rct_drawpixelinfo * RESTRICT dpi, int image_type, int source_y_start, int height, int source_x_start, int width);
void FASTCALL gfx_draw_sprite(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint32 tertiary_colour);
void FASTCALL gfx_draw_glpyh(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint8 * palette);
void FASTCALL gfx_draw_sprite_raw_masked(rct_drawpixelinfo *dpi, int x, int y, int maskImage, int colourImage);
void FASTCALL gfx_draw_sprite_solid(rct_drawpixelinfo * dpi, int image, int x, int y, uint8 colour);

void FASTCALL gfx_draw_sprite_software(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint32 tertiary_colour);
void FASTCALL gfx_draw_sprite_palette_set_software(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint8* palette_pointer, uint8* unknown_pointer);
void FASTCALL gfx_draw_sprite_raw_masked_software(rct_drawpixelinfo *dpi, int x, int y, int maskImage, int colourImage);

// string
int clip_text(char *buffer, int width);
int gfx_wrap_string(char* buffer, int width, int* num_lines, int* font_height);
int gfx_get_string_width(char *buffer);
int gfx_get_string_width_new_lined(char* buffer);
void gfx_draw_string(rct_drawpixelinfo *dpi, char *buffer, int colour, int x, int y);
void gfx_draw_string_left(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y);
void gfx_draw_string_left_clipped(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y, int width);
int gfx_draw_string_left_wrapped(rct_drawpixelinfo *dpi, void *args, int x, int y, int width, int format, int colour);
void draw_string_left_underline(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y);
void gfx_draw_string_left_centred(rct_drawpixelinfo *dpi, rct_string_id format, void *args, int colour, int x, int y);
void gfx_draw_string_centred(rct_drawpixelinfo *dpi, int format, int x, int y, int colour, void *args);
void gfx_draw_string_centred_clipped(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y, int width);
void draw_string_centred_underline(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y);
int gfx_draw_string_centred_wrapped(rct_drawpixelinfo *dpi, void *args, int x, int y, int width, int format, int colour);
void draw_string_centred_raw(rct_drawpixelinfo *dpi, int x, int y, int numLines, char *text);
void gfx_draw_string_right(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y);
void draw_string_right_underline(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y);
int string_get_height_raw(char *buffer);
void gfx_draw_string_centred_wrapped_partial(rct_drawpixelinfo *dpi, int x, int y, int width, int colour, rct_string_id format, void *args, int ticks);
void gfx_draw_string_with_y_offsets(rct_drawpixelinfo *dpi, const utf8 *text, int colour, int x, int y, const sint8 *yOffsets, bool forceSpriteFont);
int gfx_clip_string(char* buffer, int width);
void shorten_path(utf8 *buffer, size_t bufferSize, const utf8 *path, int availableWidth);

bool ttf_initialise();
void ttf_dispose();

// scrolling text
void scrolling_text_initialise_bitmaps();
int scrolling_text_setup(rct_string_id stringId, uint16 scroll, uint16 scrollingMode);

void rct2_draw(rct_drawpixelinfo *dpi);

#include "NewDrawing.h"

#endif
