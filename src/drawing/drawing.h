/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef _DRAWING_H_
#define _DRAWING_H_

#include "../common.h"

// Size: 0x10
typedef struct {
	char* bits;			// 0x00
	short x;			// 0x04
	short y;			// 0x06
	short width;		// 0x08
	short height;		// 0x0A
	short pitch;		// 0x0C			note: this is actually (pitch - width)
	uint16 zoom_level;	// 0x0E
} rct_drawpixelinfo;

// Size: 0x10
typedef struct {
	uint8* offset;			// 0x00
	sint16 width;			// 0x04
	sint16 height;			// 0x06
	sint16 x_offset;		// 0x08
	sint16 y_offset;		// 0x0A
	uint16 flags;			// 0x0C
	uint16 zoomed_offset;	// 0x0E
} rct_g1_element;

enum{
	G1_FLAG_BMP = (1 << 0), //No invisible sections
	G1_FLAG_RLE_COMPRESSION = (1<<2),
};

enum{
	IMAGE_TYPE_NO_BACKGROUND = 0,
	IMAGE_TYPE_USE_PALETTE= (1 << 1),
	IMAGE_TYPE_MIX_BACKGROUND = (1<<2),
	IMAGE_TYPE_UNKNOWN = (1<<3)
};

typedef struct {
	uint32 num_entries;
	uint32 total_size;
} rct_g1_header;

typedef struct {
	rct_g1_header header;
	rct_g1_element *elements;
	void *data;
} rct_gx;

extern const uint16 palette_to_g1_offset[];
extern const uint8 peep_palette[];
extern uint8 text_palette[];

extern int gLastDrawStringX;
extern int gLastDrawStringY;

extern rct_g1_element *g1Elements;
extern rct_gx g2;

// 
rct_drawpixelinfo* clip_drawpixelinfo(rct_drawpixelinfo* dpi, int left, int width, int top, int height);
void gfx_set_dirty_blocks(sint16 left, sint16 top, sint16 right, sint16 bottom);
void gfx_draw_all_dirty_blocks();
void gfx_redraw_screen_rect(short left, short top, short right, short bottom);
void gfx_invalidate_screen();

// palette
void gfx_transpose_palette(int pal, unsigned char product);
void load_palette();

// other
void gfx_draw_rain(int left, int top, int width, int height, sint32 x_start, sint32 y_start);
void gfx_clear(rct_drawpixelinfo *dpi, int colour);
void gfx_draw_pixel(rct_drawpixelinfo *dpi, int x, int y, int colour);
void gfx_invalidate_pickedup_peep();
void gfx_draw_pickedup_peep();

// line
void gfx_draw_line(rct_drawpixelinfo *dpi, int x1, int y1, int x2, int y2, int colour);

// rect
void gfx_fill_rect(rct_drawpixelinfo *dpi, int left, int top, int right, int bottom, int colour);
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, short left, short top, short right, short bottom, int colour, short _si);

// sprite
int gfx_load_g1();
int gfx_load_g2();
void sub_68371D();
void gfx_bmp_sprite_to_buffer(uint8* palette_pointer, uint8* unknown_pointer, uint8* source_pointer, uint8* dest_pointer, rct_g1_element* source_image, rct_drawpixelinfo *dest_dpi, int height, int width, int image_type);
void gfx_rle_sprite_to_buffer(uint8* source_bits_pointer, uint8* dest_bits_pointer, uint8* palette_pointer, rct_drawpixelinfo *dpi, int image_type, int source_y_start, int height, int source_x_start, int width);
void gfx_draw_sprite(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint32 tertiary_colour);
void gfx_draw_sprite_palette_set(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint8* palette_pointer, uint8* unknown_pointer);

// string
void gfx_load_character_widths();
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
void sub_6C1F57(rct_drawpixelinfo *dpi, int x, int y, int width, int colour, rct_string_id format, void *args, int ticks);

bool ttf_initialise();
void ttf_dispose();

// rain
void update_rain_animation();
void redraw_rain();

// unknown
void sub_681DE2(rct_drawpixelinfo *dpi, int x, int y, int image1, int image2);

#endif
