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

#ifndef _GFX_H_
#define _GFX_H_

#include "rct2.h"

// Size: 0x10
typedef struct {
	char* bits;			// 0x00
	short x;			// 0x04
	short y;			// 0x06
	short width;		// 0x08
	short height;		// 0x0A
	short pitch;		// 0x0C			note: this is actually (pitch - width)
	char pad_0E;		// 0x0E
	char var_0F;		// 0x0F
} rct_drawpixelinfo;

// Size: 0x10
typedef struct {
	uint8* offset;			// 0x00
	sint16 width;			// 0x04
	sint16 height;			// 0x06
	sint16 x_offset;		// 0x08
	sint16 y_offset;		// 0x0A
	uint16 flags;			// 0x0C
	sint16 unused;			// 0x0E
} rct_g1_element;

extern int gLastDrawStringX;
extern int gLastDrawStringY;

int gfx_load_g1();

void gfx_clear(rct_drawpixelinfo *dpi, int colour);
void gfx_draw_pixel(rct_drawpixelinfo *dpi, int x, int y, int colour);
void gfx_draw_line(rct_drawpixelinfo *dpi, int x1, int y1, int x2, int y2, int colour);
void gfx_fill_rect(rct_drawpixelinfo *dpi, int left, int top, int right, int bottom, int colour);
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, short left, short top, short right, short bottom, int colour, short _si);
void gfx_draw_sprite(rct_drawpixelinfo *dpi, int image_id, int x, int y);
void gfx_draw_string(rct_drawpixelinfo *dpi, char *format, int colour, int x, int y);
void gfx_transpose_palette(int pal, unsigned char product);

void gfx_draw_string_left(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y);
void gfx_draw_string_left_clipped(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y, int width);
void gfx_draw_string_centred_clipped(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y, int width);
void gfx_draw_string_right(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y);
void gfx_draw_string_centred(rct_drawpixelinfo *dpi, int format, int x, int y, int colour, void *args);
int gfx_draw_string_centred_wrapped(rct_drawpixelinfo *dpi, void *args, int x, int y, int width, int format, int colour);
int gfx_draw_string_left_wrapped(rct_drawpixelinfo *dpi, void *format, int x, int y, int width, int colour, int unknown);

int gfx_get_string_width(char *buffer);
int clip_text(char *buffer, int width);
void gfx_fill_rect_inset(rct_drawpixelinfo *dpi, short left, short top, short right, short bottom, int colour, short _si);

void gfx_set_dirty_blocks(int left, int top, int right, int bottom);
void gfx_draw_all_dirty_blocks();
void gfx_redraw_screen_rect(short left, short top, short right, short bottom);
void gfx_invalidate_screen();

#endif
