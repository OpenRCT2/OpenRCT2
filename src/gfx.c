/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill
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

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include "addresses.h"
#include "gfx.h"
#include "rct2.h"
#include "string_ids.h"
#include "window.h"
#include "osinterface.h"

typedef struct {
	uint32 num_entries;
	uint32 total_size;
} rct_g1_header;

void *_g1Buffer = NULL;

// HACK These were originally passed back through registers
int gLastDrawStringX;
int gLastDrawStringY;

uint8 _screenDirtyBlocks[5120];

//Originally 0x9ABE0C, 12 elements from 0xF3 are the peep top colour, 12 elements from 0xCA are peep trouser colour
uint8 peep_palette[0x100] = { 
	0x00, 0xF3, 0xF4, 0xF5, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

//Originally 0x9ABE04
uint8 text_palette[0x8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void gfx_draw_dirty_blocks(int x, int y, int columns, int rows);

/**
 * 
 *  rct2: 0x00678998
 */
int gfx_load_g1()
{
	FILE *file;
	rct_g1_header header;
	unsigned int i;

	rct_g1_element *g1Elements = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element);

	file = fopen(get_file_path(PATH_ID_G1), "rb");
	if (file != NULL) {
		if (fread(&header, 8, 1, file) == 1) {
			// number of elements is stored in g1.dat, but because the entry headers are static, this can't be variable until
			// made into a dynamic array
			header.num_entries = 29294;

			// Read element headers
			fread(g1Elements, header.num_entries * sizeof(rct_g1_element), 1, file);

			// Read element data
			_g1Buffer = rct2_malloc(header.total_size);
			fread(_g1Buffer, header.total_size, 1, file);

			fclose(file);

			// Fix entry data offsets
			for (i = 0; i < header.num_entries; i++)
				g1Elements[i].offset += (int)_g1Buffer;

			// Successful
			return 1;
		}
		fclose(file);
	}

	// Unsuccessful
	RCT2_ERROR("Unable to load g1.dat");
	return 0;
}

/**
 * Clears the screen with the specified colour.
 *  rct2: 0x00678A9F
 */
void gfx_clear(rct_drawpixelinfo *dpi, int colour)
{
	int y, w, h;
	char* ptr;

	w = dpi->width >> dpi->var_0F;
	h = dpi->height >> dpi->var_0F;

	ptr = dpi->bits;
	for (y = 0; y < h; y++) {
		memset(ptr, colour, w);
		ptr += w + dpi->pitch;
	}
}

void gfx_draw_pixel(rct_drawpixelinfo *dpi, int x, int y, int colour)
{
	gfx_fill_rect(dpi, x, y, x, y, colour);
}

/*
* Draws a horizontal line of specified colour to a buffer.
* rct2: 0x68474C
*/
void gfx_draw_line_on_buffer(rct_drawpixelinfo *dpi, char colour, int y, int x, int no_pixels)
{
	y -= dpi->y;

	//Check to make sure point is in the y range
	if (y < 0)return;
	if (y >= dpi->height)return;
	//Check to make sure we are drawing at least a pixel
	if (!no_pixels) return;

	no_pixels++;
	x -= dpi->x;

	//If x coord outside range leave
	if (x < 0){
		//Unless the number of pixels is enough to be in range
		no_pixels += x;
		if (no_pixels <= 0)return;
		//Resets starting point to 0 as we don't draw outside the range
		x = 0;
	}

	//Ensure that the end point of the line is within range
	if (x + no_pixels - dpi->width > 0){
		//If the end point has any pixels outside range
		//cut them off. If there are now no pixels return.
		no_pixels -= x + no_pixels - dpi->width;
		if (no_pixels <= 0)return;
	}

	char* bits_pointer;
	//Get the buffer we are drawing to and move to the first coordinate.
	bits_pointer = dpi->bits + y*(dpi->pitch + dpi->width) + x;

	//Draw the line to the specified colour
	for (; no_pixels > 0; --no_pixels, ++bits_pointer){
		*((uint8*)bits_pointer) = colour;
	}
}


/**
 * Draws a line on dpi if within dpi boundaries
 *  rct2: 0x00684466
 * dpi (edi)
 * x1 (ax)
 * y1 (bx)
 * x2 (cx)
 * y2 (dx)
 * colour (ebp)
 */
void gfx_draw_line(rct_drawpixelinfo *dpi, int x1, int y1, int x2, int y2, int colour)
{
	// Check to make sure the line is within the drawing area
	if ((x1 < dpi->x) && (x2 < dpi->x)){
		return;
	}

	if ((y1 < dpi->y) && (y2 < dpi->y)){
		return;
	}

	if ((x1 >(dpi->x + dpi->width)) && (x2 >(dpi->x + dpi->width))){
		return;
	}

	if ((y1 > (dpi->y + dpi->height)) && (y2 > (dpi->y + dpi->height))){
		return;
	}

	//Bresenhams algorithm

	//If vertical plot points upwards
	int steep = abs(y2 - y1) > abs(x2 - x1);
	if (steep){
		int temp_y2 = y2;
		int temp_x2 = x2;
		y2 = x1;
		x2 = y1;
		y1 = temp_x2;
		x1 = temp_y2;
	}

	//If line is right to left swap direction
	if (x1 > x2){
		int temp_y2 = y2;
		int temp_x2 = x2;
		y2 = y1;
		x2 = x1;
		y1 = temp_y2;
		x1 = temp_x2;
	}

	int delta_x = x2 - x1;
	int delta_y = abs(y2 - y1);
	int error = delta_x / 2;
	int y_step;
	int y = y1;

	//Direction of step
	if (y1 < y2)y_step = 1;
	else y_step = -1;

	for (int x = x1, x_start = x1, no_pixels = 1; x < x2; ++x,++no_pixels){
		//Vertical lines are drawn 1 pixel at a time
		if (steep)gfx_draw_line_on_buffer(dpi, colour, x, y, 1);

		error -= delta_y;
		if (error < 0){
			//Non vertical lines are drawn with as many pixels in a horizontal line as possible
			if (!steep)gfx_draw_line_on_buffer(dpi, colour, y, x_start, no_pixels);

			//Reset non vertical line vars
			x_start = x + 1;
			no_pixels = 1;
			y += y_step;
			error += delta_x;
		}

		//Catch the case of the last line
		if (x + 1 == x2 && !steep){
			gfx_draw_line_on_buffer(dpi, colour, y, x_start, no_pixels);
		}
	}
	return;
}

/**
 *
 *  rct2: 0x00678AD4
 * dpi (edi)
 * left (ax)
 * top (cx)
 * right (bx)
 * bottom (dx)
 * colour (ebp)
 */
void gfx_fill_rect(rct_drawpixelinfo *dpi, int left, int top, int right, int bottom, int colour)
{
	int left_, right_, top_, bottom_;
	rct_drawpixelinfo* dpi_;
	left_ = left;
	right_ = right;
	top_ = top;
	bottom_ = bottom;
	dpi_ = dpi;
  
	if ((left > right) || (top > bottom) || (dpi->x > right) || (left >= (dpi->x + dpi->width)) ||
		(bottom < dpi->y) || (top >= (dpi->y + dpi->height)))
		return;

	colour |= RCT2_GLOBAL(0x009ABD9C, uint32);

	if (!(colour & 0x1000000)) {
		if (!(colour & 0x8000000)) {
			left_ = left - dpi->x;
			if (left_ < 0)
				left_ = 0;

			right_ = right - dpi->x;
			right_++;
			if (right_ > dpi->width)
				right_ = dpi->width;

			right_ -= left_;
	
			top_ = top - dpi->y;
			if (top_ < 0)
				top_ = 0;

			bottom_ = bottom - dpi->y;
			bottom_++;

			if (bottom_ > dpi->height)
				bottom_ = dpi->height;

			bottom_ -= top_;

			if (!(colour & 0x2000000)) {
				if (!(colour & 0x4000000)) {
					uint8* pixel = (top_ * (dpi->width + dpi->pitch)) + left_ + dpi->bits;

					int length = dpi->width + dpi->pitch - right_;

					for (int i = 0; i < bottom_; ++i) {
						memset(pixel, (colour & 0xFF), right_);
						pixel += length + right_;
					}
				} else {
					// 00678B8A   00678E38
					char* esi;
					esi = (top_ * (dpi->width + dpi->pitch)) + left_ + dpi->bits;;

					int eax, ebp;
					eax = colour;
					ebp = dpi->width + dpi->pitch - right_;

					RCT2_GLOBAL(0x00EDF810, uint32) = ebp;
					RCT2_GLOBAL(0x009ABDB2, uint16) = bottom_;
					RCT2_GLOBAL(0x00EDF814, uint32) = right_;

					top_ = (top + dpi->y) & 0xf;
					right_ = (right + dpi_->x) &0xf;

					dpi_ = (rct_drawpixelinfo*)esi;

					esi = (char*)(eax >> 0x1C);
					esi = (char*)RCT2_GLOBAL(0x0097FEFC,uint32)[esi]; // or possibly uint8)[esi*4] ?

					for (; RCT2_GLOBAL(0x009ABDB2, uint16) > 0; RCT2_GLOBAL(0x009ABDB2, uint16)--) {
						// push    ebx
						// push    ecx
						ebp = *(esi + top_*2);
					     
						// mov     bp, [esi+top_*2];
						int ecx;
						ecx = RCT2_GLOBAL(0x00EDF814, uint32);

						for (int i = ecx; i >=0; --i) {
							if (!(ebp & (1 << right_)))
								dpi_->bits = (char*)(left_ & 0xFF);
		
							right_++;
							right_ = right_ & 0xF;
							dpi_++;
						}
						// pop     ecx
						// pop     ebx
						top_++;
						top_ = top_ &0xf;
						dpi_ += RCT2_GLOBAL(0x00EDF810, uint32);
					}
					return;
				}

			} else {
				// 00678B7E   00678C83
				if (dpi->zoom_level < 1) {
					// Location in screen buffer?
					uint8* pixel = top_ * (dpi->width + dpi->pitch) + left_ + dpi->bits;

					// Find colour in colour table?
					uint32 eax = RCT2_ADDRESS(0x0097FCBC, uint32)[(colour & 0xFF)];
					rct_g1_element* g1_element = &(RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[eax]);

					int length = (dpi->width + dpi->pitch) - right_;

					// Fill the rectangle with the colours from the colour table
					for (int i = 0; i < bottom_; ++i) {
						for (int j = 0; j < right_; ++j) {
							*pixel = *((uint8*)(&g1_element->offset[*pixel]));
							pixel++;
						}
						pixel += length;
					}
				} else if (dpi->zoom_level > 1) {
					// 00678C8A    00678D57
					right_ = right;
				} else if (dpi->zoom_level == 1) {
					// 00678C88   00678CEE
					right = right;
				}
	
			}
		} else {
			// 00678B3A    00678EC9
			right_ = right;
      }
  } else {
    // 00678B2E    00678BE5
		// Cross hatching
		uint16 pattern = 0;

		left_ = left_ - dpi->x;
		if (left_ < 0) {
			pattern = pattern ^ left_;
			left_ = 0;
		}

		right_ = right_ - dpi->x;
		right_++;

		if (right_ > dpi->width)
			right_ = dpi-> width;
		
		right_ = right_ - left_;

		top_ = top - dpi->y;
		if (top_ < 0) {
			pattern = pattern ^ top_;
			top_ = 0;
		}

		bottom_ = bottom - dpi->y;
		bottom_++;

		if (bottom_ > dpi->height)
			bottom_ = dpi->height;

		bottom_ -= top_;

		uint8* pixel = (top_ * (dpi->width + dpi->pitch)) + left_ + dpi->bits;

		int length = dpi->width + dpi->pitch - right_;

		uint32 ecx;
		for (int i = 0; i < bottom_; ++i) {
			ecx = pattern;
			// Rotate right
			ecx = (ecx >> 1) | (ecx << (sizeof(ecx) * CHAR_BIT - 1));
			ecx = (ecx & 0xFFFF0000) | right_; 
			// Fill every other pixel with the colour
			for (; (ecx & 0xFFFF) > 0; ecx--) {
				ecx = ecx ^ 0x80000000;
				if ((int)ecx < 0) {
					*pixel = colour & 0xFF;
				}
				pixel++;
			}
			pattern = pattern ^ 1;
			pixel += length;
			
		}
	}

	// RCT2_CALLPROC_X(0x00678AD4, left, right, top, bottom, 0, dpi, colour);
}

/**
 *  Draw a rectangle, with optional border or fill
 *
 *  rct2: 0x006E6F81
 * dpi (edi)
 * left (ax)
 * top (cx)
 * right (bx)
 * bottom (dx)
 * colour (ebp)
 * flags (si)
 */
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, short left, short top, short right, short bottom, int colour, short flags)
{
	uint8 shadow, fill, hilight;

	// Flags
	int no_border, no_fill, pressed;

	no_border = 8;
	no_fill = 0x10;
	pressed = 0x20;

	if (colour & 0x180) {
		if (colour & 0x100) {
			colour = colour & 0x7F;
		} else {
			colour = RCT2_ADDRESS(0x009DEDF4,uint8)[colour];
		}

		colour = colour | 0x2000000; //Transparent

		if (flags & no_border) {
			gfx_fill_rect(dpi, left, top, bottom, right, colour);
		} else if (flags & pressed) {
			// Draw outline of box
			gfx_fill_rect(dpi, left, top, left, bottom, colour + 1);
			gfx_fill_rect(dpi, left, top, right, top, colour + 1);
			gfx_fill_rect(dpi, right, top, right, bottom, colour + 2);
			gfx_fill_rect(dpi, left, bottom, right, bottom, colour + 2);

			if (!(flags & no_fill)) {
				gfx_fill_rect(dpi, left+1, top+1, right-1, bottom-1, colour);
			}
		} else {
			// Draw outline of box
			gfx_fill_rect(dpi, left, top, left, bottom, colour + 2);
			gfx_fill_rect(dpi, left, top, right, top, colour + 2);
			gfx_fill_rect(dpi, right, top, right, bottom, colour + 1);
			gfx_fill_rect(dpi, left, bottom, right, bottom, colour + 1);

			if (!(flags & no_fill)) {
				gfx_fill_rect(dpi, left+1, top+1, right-1, bottom-1, colour);
			}
		}
	} else {
		if (flags & 0x80) {
			shadow	= RCT2_ADDRESS(0x0141FC46, uint8)[colour * 8];
			fill	= RCT2_ADDRESS(0x0141FC48, uint8)[colour * 8];
			hilight	= RCT2_ADDRESS(0x0141FC4A, uint8)[colour * 8];
		} else {
			shadow	= RCT2_ADDRESS(0x0141FC47, uint8)[colour * 8];
			fill	= RCT2_ADDRESS(0x0141FC49, uint8)[colour * 8];
			hilight	= RCT2_ADDRESS(0x0141FC4B, uint8)[colour * 8];
		}

		if (flags & no_border) {
			gfx_fill_rect(dpi, left, top, right, bottom, fill);
		} else if (flags & pressed) {
			// Draw outline of box
			gfx_fill_rect(dpi, left, top, left, bottom, shadow);
			gfx_fill_rect(dpi, left + 1, top, right, top, shadow);
			gfx_fill_rect(dpi, right, top + 1, right, bottom - 1, hilight);
			gfx_fill_rect(dpi, left + 1, bottom, right, bottom, hilight);

			if (!(flags & no_fill)) {
				if (!(flags & 0x40)) {
					if (flags & 0x04) {
						fill = RCT2_ADDRESS(0x0141FC49, uint8)[0];
					} else {
						fill = RCT2_ADDRESS(0x0141FC4A, uint8)[colour * 8];
					}
				}
				gfx_fill_rect(dpi, left+1, top+1, right-1, bottom-1, fill);
			}
		} else {
			// Draw outline of box
			gfx_fill_rect(dpi, left, top, left, bottom - 1, hilight);
			gfx_fill_rect(dpi, left + 1, top, right - 1, top, hilight);
			gfx_fill_rect(dpi, right, top, right, bottom - 1, shadow);
			gfx_fill_rect(dpi, left, bottom, right, bottom, shadow);

			if (!(flags & no_fill)) {
				if (flags & 0x04) {
					fill = RCT2_ADDRESS(0x0141FC49, uint8)[0];
				}
				gfx_fill_rect(dpi, left+1, top+1, right-1, bottom-1, fill);
			}
		}
	}
}

#define RCT2_Y_RELATED_GLOBAL_1 0x9E3D12 //uint16
#define RCT2_Y_END_POINT_GLOBAL 0x9ABDAC //sint16
#define RCT2_Y_START_POINT_GLOBAL 0xEDF808 //sint16
#define RCT2_X_RELATED_GLOBAL_1 0x9E3D10 //uint16
#define RCT2_X_END_POINT_GLOBAL 0x9ABDA8 //sint16
#define RCT2_X_START_POINT_GLOBAL 0xEDF80C //sint16
#define RCT2_DPI_LINE_LENGTH_GLOBAL 0x9ABDB0 //uint16 width+pitch

/*
* rct2: 0x67A690
* copies a sprite onto the buffer. There is no compression used on the sprite
* image.
*/
void gfx_bmp_sprite_to_buffer(uint8* palette_pointer, uint8* unknown_pointer, uint8* source_pointer, uint8* dest_pointer, rct_g1_element* source_image, rct_drawpixelinfo *dest_dpi, int height, int width, int image_type){
	uint8 zoom_level = dest_dpi->zoom_level + 1;
	//Requires use of palette?
	if (image_type & IMAGE_TYPE_USE_PALETTE){

		//Mix with another image?? and colour adjusted
		if (unknown_pointer!= NULL){ //Not tested. I can't actually work out when this code runs.
			unknown_pointer += source_pointer - source_image->offset;// RCT2_GLOBAL(0x9E3CE0, uint32);

			for (; height > 0; height -= zoom_level){
				uint8* next_source_pointer = source_pointer + source_image->width*zoom_level;
				uint8* next_unknown_pointer = unknown_pointer + source_image->width*zoom_level;
				uint8* next_dest_pointer = dest_pointer + dest_dpi->width + dest_dpi->pitch;

				for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_level, source_pointer+=zoom_level,  unknown_pointer+=zoom_level, dest_pointer++){
					uint8 pixel = *source_pointer;
					pixel = palette_pointer[pixel];
					pixel &= *unknown_pointer;
					if (pixel){
						*dest_pointer = pixel;
					}
				}
				source_pointer = next_source_pointer;
				dest_pointer = next_dest_pointer;
				unknown_pointer = next_unknown_pointer;
			}
			return;
		}

		//image colour adjusted?
		for (; height > 0; height -= zoom_level){
			uint8* next_source_pointer = source_pointer + source_image->width*zoom_level;
			uint8* next_dest_pointer = dest_pointer + dest_dpi->width + dest_dpi->pitch;
			for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_level, source_pointer+= zoom_level, dest_pointer++){
				uint8 pixel = *source_pointer;
				pixel = palette_pointer[pixel];
				if (pixel){
					*dest_pointer = pixel;
				}
			}

			source_pointer = next_source_pointer;
			dest_pointer = next_dest_pointer;
		}
		return;
	}

	//Mix with background. It only uses source pointer for
	//telling if it needs to be drawn not for colour.
	if (image_type & IMAGE_TYPE_MIX_BACKGROUND){//Not tested
		for (; height > 0; height -= zoom_level){
			uint8* next_source_pointer = source_pointer + source_image->width*zoom_level;
			uint8* next_dest_pointer = dest_pointer + dest_dpi->width + dest_dpi->pitch;

			for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_level, source_pointer += zoom_level, dest_pointer++){
				uint8 pixel = *source_pointer;
				if (pixel){
					pixel = *dest_pointer;
					pixel = palette_pointer[pixel];
					*dest_pointer = pixel;
				}
			}

			source_pointer = next_source_pointer;
			dest_pointer = next_dest_pointer;
		}
		return;
	}

	//Basic bitmap no fancy stuff
	if (!(source_image->flags & G1_FLAG_BMP)){//Not tested
		for (; height > 0; height-=zoom_level){
			uint8* next_source_pointer = source_pointer + source_image->width*zoom_level;
			uint8* next_dest_pointer = dest_pointer + dest_dpi->width + dest_dpi->pitch;

			for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_level, dest_pointer++, source_pointer += zoom_level){
				*dest_pointer = *source_pointer;
			}

			dest_pointer = next_dest_pointer;
			source_pointer = next_source_pointer;
		}
		return;
	}

	if (RCT2_GLOBAL(0x9E3CDC, uint32) != 0){//Not tested. I can't actually work out when this code runs.
		unknown_pointer += source_pointer - source_image->offset;

		for (; height > 0; height -= zoom_level){
			uint8* next_source_pointer = source_pointer + source_image->width*zoom_level;
			uint8* next_unknown_pointer = unknown_pointer + source_image->width*zoom_level;
			uint8* next_dest_pointer = dest_pointer + dest_dpi->width + dest_dpi->pitch;

			for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_level, dest_pointer++, source_pointer += zoom_level, unknown_pointer += zoom_level){
				uint8 pixel = *source_pointer;
				pixel &= *unknown_pointer;
				if (pixel){
					*dest_pointer = pixel;
				}
			}
			dest_pointer = next_dest_pointer;
			source_pointer = next_source_pointer;
			unknown_pointer = next_unknown_pointer;
		}
	}

	//Basic bitmap with no draw pixels
	for (; height > 0; height -= zoom_level){
		uint8* next_source_pointer = source_pointer + source_image->width*zoom_level;
		uint8* next_dest_pointer = dest_pointer + dest_dpi->width + dest_dpi->pitch;

		for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_level, dest_pointer++, source_pointer += zoom_level){
			uint8 pixel = *source_pointer;
			if (pixel){
				*dest_pointer = pixel;
			}
		}
		dest_pointer = next_dest_pointer;
		source_pointer = next_source_pointer;
	}
	return;
}


/*
* rct2: 0x67AA18 transfers readied images onto buffers
* This function copies the sprite data onto the screen
* I think its only used for bitmaps onto buttons but i am not sure.
* There is still a small bug with this code when it is in the choose park view.
*/
void gfx_rle_sprite_to_buffer(uint8* source_bits_pointer, uint8* dest_bits_pointer, uint8* palette_pointer, rct_drawpixelinfo *dpi, int image_type, int source_y_start, int height, int source_x_start, int width){
	int zoom_level = dpi->zoom_level + 1;

	uint8* next_source_pointer;
	uint8* next_dest_pointer = dest_bits_pointer;

	//For every line in the image
	for (int y = source_y_start; y < (height + source_y_start); y += zoom_level){

		//The first part of the source pointer is a list of offsets to different lines
		//This will move the pointer to the correct source line.
		next_source_pointer = source_bits_pointer + ((uint16*)source_bits_pointer)[y];

		uint8 last_data_line = 0;

		//For every data section in the line
		while (!last_data_line){
			uint8* source_pointer = next_source_pointer;
			uint8* dest_pointer = next_dest_pointer;

			int no_pixels = *source_pointer++;
			//gap_size is the number of non drawn pixels you require to
			//jump over on your destination
			uint8 gap_size = *source_pointer++;
			//The last bit in no_pixels tells you if you have reached the end of a line
			last_data_line = no_pixels & 0x80;
			//Clear the last data line bit so we have just the no_pixels
			no_pixels &= 0x7f;
			//Have our next source pointer point to the next data section
			next_source_pointer = source_pointer + no_pixels;

			//Calculates the start point of the image
			int x_start = gap_size - source_x_start;

			if (x_start > 0){
				//Since the start is positive
				//We need to move the drawing surface to the correct position
				dest_pointer += x_start / zoom_level;
			}
			else{
				//If the start is negative we require to remove part of the image.
				//This is done by moving the image pointer to the correct position.
				source_pointer -= x_start;
				//The no_pixels will be reduced in this operation
				no_pixels += x_start;
				//If there are no pixels there is nothing to draw this data section
				if (no_pixels <= 0) continue;
				//Reset the start position to zero as we have taken into account all moves
				x_start = 0;
			}

			int x_end = x_start + no_pixels;
			//If the end position is further out than the whole image
			//end position then we need to shorten the line again
			if (x_end > width){
				//Shorten the line
				no_pixels -= x_end - width;
				//If there are no pixels there is nothing to draw.
				if (no_pixels <= 0) continue;
			}

			//Finally after all those checks, copy the image onto the drawing surface
			//If the image type is not a basic one we require to mix the pixels
			if (image_type & IMAGE_TYPE_USE_PALETTE){//In the .exe these are all unraveled loops
				for (; no_pixels > 0; no_pixels -= zoom_level, source_pointer += zoom_level, dest_pointer++){
					uint8 al = *source_pointer;
					uint8 ah = *dest_pointer;
					if (image_type & IMAGE_TYPE_MIX_BACKGROUND)//Mix with background and image Not Tested
						al = palette_pointer[(al | ((int)ah) << 8) - 0x100];
					else //Adjust colours?
						al = palette_pointer[al];
					*dest_pointer = al;
				}
			}
			else if (image_type & IMAGE_TYPE_MIX_BACKGROUND){//In the .exe these are all unraveled loops
				//Doesnt use source pointer ??? mix with background only?
				//Not Tested
				for (; no_pixels > 0; no_pixels -= zoom_level, dest_pointer++){
					uint8 pixel = *dest_pointer;
					pixel = palette_pointer[pixel];
					*dest_pointer = pixel;
				}
			}
			else
			{
				for (; no_pixels > 0; no_pixels -= zoom_level, source_pointer += zoom_level, dest_pointer++){
					*dest_pointer = *source_pointer;
				}
			}
		}

		//Add a line to the drawing surface pointer
		next_dest_pointer += (int)dpi->width + (int)dpi->pitch;
	}
}


void gfx_draw_sprite_palette_set(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint8* palette_pointer, uint8* unknown_pointer);
/**
 *
 *  rct2: 0x0067A28E
 * image_id (ebx)
 * x (cx)
 * y (dx)
 */
void gfx_draw_sprite(rct_drawpixelinfo *dpi, int image_id, int x, int y)
{

	int eax = 0, ebx = image_id, ecx = x, edx = y, esi = 0, edi = (int)dpi, ebp = 0;
	int image_type = (image_id & 0xE0000000) >> 28;
	int image_sub_type = (image_id & 0x1C000000) >> 26;
	uint8* palette_pointer = NULL;
	uint8 palette[0x100];
	RCT2_GLOBAL(0x00EDF81C, uint32) = image_id & 0xE0000000;
	eax = (image_id >> 26) & 0x7;

	uint8* unknown_pointer = (uint8*)(RCT2_ADDRESS(0x9E3CE4, uint32*)[image_sub_type]);
	RCT2_GLOBAL(0x009E3CDC, uint32) = RCT2_GLOBAL(0x009E3CE4 + eax * 4, uint32);

	if (image_type && !(image_type & IMAGE_TYPE_UNKNOWN)) {

		if (!(image_type & IMAGE_TYPE_MIX_BACKGROUND)){
			eax = image_id;
			eax >>= 19;
			eax &= 0xFF;
			unknown_pointer = NULL;
			RCT2_GLOBAL(0x009E3CDC, uint32) = 0;
		}
		else{
			eax = image_id;
			eax >>= 19;
			eax &= 0x7F;
		}
		eax = RCT2_GLOBAL(eax * 4 + 0x97FCBC, uint32);

		palette_pointer = ((rct_g1_element*)RCT2_ADDRESS_G1_ELEMENTS)[eax].offset;
		RCT2_GLOBAL(0x9ABDA4, uint32) = (uint32)palette_pointer;
	}
	else if (image_type && !(image_type & IMAGE_TYPE_USE_PALETTE)){
		//Has not been tested
		RCT2_GLOBAL(0x9E3CDC, uint32) = 0;
		unknown_pointer = NULL;

		eax = image_id;
		eax >>= 19;
		//push edx/y
		eax &= 0x1F;
		ebp = RCT2_GLOBAL(ebp * 4 + 0x97FCBC, uint32);
		eax = RCT2_GLOBAL(eax * 4 + 0x97FCBC, uint32);
		ebp <<= 0x4;
		eax <<= 0x4;
		ebp = RCT2_GLOBAL(ebp + RCT2_ADDRESS_G1_ELEMENTS, uint32);
		eax = RCT2_GLOBAL(eax + RCT2_ADDRESS_G1_ELEMENTS, uint32);
		edx = *((uint32*)(eax + 0xF3));
		esi = *((uint32*)(eax + 0xF7));
		RCT2_GLOBAL(0x9ABFFF, uint32) = edx;
		RCT2_GLOBAL(0x9AC003, uint32) = esi;
		edx = *((uint32*)(ebp + 0xF3));
		esi = *((uint32*)(ebp + 0xF7));
		esi = *((uint32*)(eax + 0xF7));

		edx = *((uint32*)(eax + 0xFB));
		esi = *((uint32*)(ebp + 0xFB));

		eax = image_id;
		RCT2_GLOBAL(0x9AC007, uint32) = edx;
		eax >>= 24;
		RCT2_GLOBAL(0x9ABF42, uint32) = esi;
		eax &= 0x1F;

		//image_id
		RCT2_GLOBAL(0xEDF81C, uint32) |= 0x20000000;
		image_id |= IMAGE_TYPE_USE_PALETTE;

		eax = RCT2_GLOBAL(eax * 4 + 0x97FCBC, uint32);
		eax <<= 4;
		eax = RCT2_GLOBAL(eax + RCT2_ADDRESS_G1_ELEMENTS, uint32);
		edx = *((uint32*)(eax + 0xF3));
		esi = *((uint32*)(eax + 0xF7));
		RCT2_GLOBAL(0x9ABFD6, uint32) = edx;
		RCT2_GLOBAL(0x9ABFDA, uint32) = esi;
		edx = *((uint32*)(eax + 0xFB));
		RCT2_GLOBAL(0x9ABDA4, uint32) = 0x9ABF0C;
		palette_pointer = (uint8*)0x9ABF0C;
		RCT2_GLOBAL(0x9ABFDE, uint32) = edx;
		edx = y;

	}
	else if (image_type){
		RCT2_GLOBAL(0x9E3CDC, uint32) = 0;
		unknown_pointer = NULL;
		//Copy the peep palette into a new palette. 
		//Not really required but its nice to make a copy
		memcpy(palette, peep_palette, 0x100);

		//Top
		int top_type = (image_id >> 19) & 0x1f;
		uint32 top_offset = RCT2_ADDRESS(0x97FCBC, uint32)[top_type];
		rct_g1_element top_palette = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[top_offset];
		memcpy(palette + 0xF3, top_palette.offset + 0xF3, 12);
		
		//Trousers
		int trouser_type = (image_id >> 24) & 0x1f;
		uint32 trouser_offset = RCT2_ADDRESS(0x97FCBC, uint32)[trouser_type];
		rct_g1_element trouser_palette = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[trouser_offset];
		memcpy(palette + 0xCA, trouser_palette.offset + 0xF3, 12);

		//For backwards compatibility until the zooming function is done
		RCT2_GLOBAL(0x9ABDA4, uint8*) = palette;
		palette_pointer = palette;
	}

	gfx_draw_sprite_palette_set(dpi, image_id, x, y, palette_pointer, unknown_pointer);
}

/*
* 0x67A46E
*/
void gfx_draw_sprite_palette_set(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint8* palette_pointer, uint8* unknown_pointer){
	int image_element = 0x7FFFF&image_id;
	int image_type = (image_id & 0xE0000000) >> 28;
	
	rct_g1_element* g1_source = &(RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[image_element]);

	//Zooming code has been integrated into main code.
	/*if (dpi->zoom_level >= 1){ //These have not been tested
		//something to do with zooming
		if (dpi->zoom_level == 1){
			RCT2_CALLPROC_X(0x0067BD81, 0, (int)g1_source, x, y, 0,(int) dpi, 0);
			return;
		}
		if (dpi->zoom_level == 2){
			RCT2_CALLPROC_X(0x0067DADA, 0, (int)g1_source, x, y, 0, (int)dpi, 0);
			return;
		}
		RCT2_CALLPROC_X(0x0067FAAE, 0, (int)g1_source, x, y, 0, (int)dpi, 0);
		return;
	}*/

	//We add on one so that divides will create the correct number of pixels
	int zoom_level = dpi->zoom_level + 1;
	//This will be the height of the drawn image
	int height = g1_source->height / zoom_level;
	//This is the start y coordinate on the destination
	int dest_start_y = y - dpi->y + g1_source->y_offset;
	//This is the start y coordinate on the source
	int source_start_y = 0;

	
	if (dest_start_y < 0){
		//If the destination y is negative reduce the height of the
		//image as we will cut off the bottom
		height += dest_start_y;
		//If the image is no longer visible nothing to draw
		if (height <= 0){
			return;
		}
		//The source image will start a further up the image
		source_start_y -= dest_start_y*zoom_level;
		//The destination start is now reset to 0
		dest_start_y = 0;
	}
	

	int dest_end_y = dest_start_y + height;

	if (dest_end_y > dpi->height){
		//If the destination y is outside of the drawing
		//image reduce the height of the image
		height -= dest_end_y - dpi->height;
		//If the image no longer has anything to draw
		if (height <= 0)return;
	}
	
	//This will be the width of the drawn image
	int width = g1_source->width / zoom_level;
	//This is the source start x coordinate
	int source_start_x = 0;
	//This is the destination start x coordinate
	int dest_start_x = x - dpi->x + g1_source->x_offset;
	
	if (dest_start_x < 0){
		//If the destination is negative reduce the width
		//image will cut off the side
		width += dest_start_x;
		//If there is no image to draw
		if (width <= 0){
			return;
		}
		//The source start will also need to cut off the side
		source_start_x -= dest_start_x*zoom_level;
		//Reset the destination to 0
		dest_start_x = 0;
	}

	int dest_end_x = dest_start_x + width;

	if (dest_end_x > dpi->width){
		//If the destination x is outside of the drawing area
		//reduce the image width.
		width -= dest_end_x - dpi->width;
		//If there is no image to draw.
		if (width <= 0)return;
	}

	
	uint8* dest_pointer = (uint8*)dpi->bits;
	//Move the pointer to the start point of the destination
	dest_pointer += (dpi->width + dpi->pitch)*dest_start_y + dest_start_x;

	height *= zoom_level;
	width *= zoom_level;

	if (g1_source->flags & G1_FLAG_RLE_COMPRESSION){
		//We have to use a different method to move the source pointer for
		//rle encoded sprites so that will be handled within this function
		gfx_rle_sprite_to_buffer(g1_source->offset, dest_pointer, palette_pointer, dpi, image_type, source_start_y, height, source_start_x, width);
		return;
	}
	uint8* source_pointer = g1_source->offset;
	//Move the pointer to the start point of the source
	source_pointer += g1_source->width*source_start_y + source_start_x;

	if (!(g1_source->flags & 0x02)){
		gfx_bmp_sprite_to_buffer(palette_pointer, unknown_pointer, source_pointer, dest_pointer, g1_source, dpi, height, width, image_type);
		return;
	}
	//0x67A60A Not tested
	int total_no_pixels = g1_source->width*g1_source->height;
	source_pointer = g1_source->offset;
	uint8* new_source_pointer_start = malloc(total_no_pixels);
	uint8* new_source_pointer = new_source_pointer_start;// 0x9E3D28;
	int ebx, ecx;
	while (total_no_pixels>0){
		sint8 no_pixels = *source_pointer;
		if (no_pixels >= 0){
			source_pointer++;
			total_no_pixels -= no_pixels;
			memcpy((char*)new_source_pointer, (char*)source_pointer, no_pixels);
			new_source_pointer += no_pixels;
			source_pointer += no_pixels;
			continue;
		}
		ecx = no_pixels;
		no_pixels &= 0x7;
		ecx >>= 3;//SAR
		int eax = ((int)no_pixels)<<8;
		ecx = -ecx;//Odd
		eax = eax & 0xFF00 + *(source_pointer+1);
		total_no_pixels -= ecx;
		source_pointer += 2;
		ebx = (uint32)new_source_pointer - eax;
		eax = (uint32)source_pointer;
		source_pointer = (uint8*)ebx;
		ebx = eax;
		eax = 0;
		memcpy((char*)new_source_pointer, (char*)source_pointer, ecx);
		new_source_pointer += ecx;
		source_pointer += ecx;
		source_pointer = (uint8*)ebx;
	}
	source_pointer = new_source_pointer_start + g1_source->width*source_start_y + source_start_x;
	gfx_bmp_sprite_to_buffer(palette_pointer, unknown_pointer, source_pointer, dest_pointer, g1_source, dpi, height, width, image_type);
	free(new_source_pointer_start);
	return;
}

/**
 *
 *  rct2: 0x00683854
 * a1 (ebx)
 * product (cl)
 */
void gfx_transpose_palette(int pal, unsigned char product)
{
	int eax, ebx, ebp;
	uint8* esi, *edi;

	ebx = pal * 16;
	esi = (uint8*)(*((int*)(RCT2_ADDRESS_G1_ELEMENTS + ebx)));
	ebp = *((short*)(0x009EBD2C + ebx));
	eax = *((short*)(0x009EBD30 + ebx)) * 4;
	edi = (uint8*)0x01424680 + eax;

	for (; ebp > 0; ebp--) {
		edi[0] = (esi[0] * product) >> 8;
		edi[1] = (esi[1] * product) >> 8;
		edi[2] = (esi[2] * product) >> 8;
		esi += 3;
		edi += 4;
	}
	osinterface_update_palette((char*)0x01424680, 10, 236);
}

/**
 * Draws i formatted text string centred at i specified position.
 *  rct2: 0x006C1D6C
 * dpi (edi)
 * format (bx)
 * x (cx)
 * y (dx)
 * colour (al)
 * args (esi)
 */
void gfx_draw_string_centred(rct_drawpixelinfo *dpi, int format, int x, int y, int colour, void *args)
{
	char* buffer;
	short text_width;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 0xE0;

	// Measure text width
	text_width = gfx_get_string_width(buffer);

	// Draw the text centred
	if (text_width <= 0xFFFF) {
		x -= text_width / 2;
		gfx_draw_string(dpi, buffer, colour, x, y);
	}
}

/**
 *
 *  rct2: 0x006ED7E5
 */
void gfx_invalidate_screen()
{
	int width = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	int height = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16);
	gfx_set_dirty_blocks(0, 0, width, height);
}

/**
 * 
 *  rct2: 0x006E732D
 * left (ax)
 * top (bx)
 * right (dx)
 * bottom (bp)
 */
void gfx_set_dirty_blocks(int left, int top, int right, int bottom)
{
	int x, y;
	uint8 *screenDirtyBlocks = RCT2_ADDRESS(0x00EDE408, uint8);

	left = max(left, 0);
	top = max(top, 0);
	right = min(right, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16));
	bottom = min(bottom, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16));

	if (left >= right)
		return;
	if (top >= bottom)
		return;

	right--;
	bottom--;

	left >>= RCT2_GLOBAL(0x009ABDF0, sint8);
	right >>= RCT2_GLOBAL(0x009ABDF0, sint8);
	top >>= RCT2_GLOBAL(0x009ABDF1, sint8);
	bottom >>= RCT2_GLOBAL(0x009ABDF1, sint8);

	for (y = top; y <= bottom; y++)
		for (x = left; x <= right; x++)
			screenDirtyBlocks[y * RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_COLUMNS, sint32) + x] = 0xFF;
}

/**
 *
 *  rct2: 0x006E73BE
 */
void gfx_draw_all_dirty_blocks()
{
	int x, y, xx, yy, columns, rows;
	uint8 *screenDirtyBlocks = RCT2_ADDRESS(0x00EDE408, uint8);

	for (x = 0; x < RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_COLUMNS, sint32); x++) {
		for (y = 0; y < RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_ROWS, sint32); y++) {
			if (screenDirtyBlocks[y * RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_COLUMNS, sint32) + x] == 0)
				continue;

			// Determine columns
			for (xx = x; xx < RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_COLUMNS, sint32); xx++)
				if (screenDirtyBlocks[y * RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_COLUMNS, sint32) + xx] == 0)
					break;
			columns = xx - x;

			// Check rows
			for (yy = y; yy < RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_ROWS, sint32); yy++)
				for (xx = x; xx < x + columns; xx++)
					if (screenDirtyBlocks[yy * RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_COLUMNS, sint32) + xx] == 0)
						goto endRowCheck;
			
		endRowCheck:
			rows = yy - y;
			gfx_draw_dirty_blocks(x, y, columns, rows);
		}
	}
}

static void gfx_draw_dirty_blocks(int x, int y, int columns, int rows)
{
	int left, top, right, bottom;
	uint8 *screenDirtyBlocks = RCT2_ADDRESS(0x00EDE408, uint8);

	// Unset dirty blocks
	for (top = y; top < y + rows; top++)
		for (left = x; left < x + columns; left++)
			screenDirtyBlocks[top * RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_COLUMNS, sint32) + left] = 0;

	// Determine region in pixels
	left = max(0, x * RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_WIDTH, sint16));
	top = max(0, y * RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_HEIGHT, sint16));
	right = min(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16), left + (columns * RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_WIDTH, sint16)));
	bottom = min(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16), top + (rows * RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_HEIGHT, sint16)));
	if (right <= left || bottom <= top)
		return;

	// Draw region
	gfx_redraw_screen_rect(left, top, right, bottom);
}

/**
 * 
 *  rct2: 0x006E7499 
 * left (ax)
 * top (bx)
 * right (dx)
 * bottom (bp)
 */
void gfx_redraw_screen_rect(short left, short top, short right, short bottom)
{
	rct_window* w;
	rct_drawpixelinfo *screenDPI = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	rct_drawpixelinfo *windowDPI = RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_DPI, rct_drawpixelinfo);

	// Unsure what this does
	RCT2_CALLPROC_X(0x00683326, left, top, right - 1, bottom - 1, 0, 0, 0);

	windowDPI->bits = screenDPI->bits + left + ((screenDPI->width + screenDPI->pitch) * top);
	windowDPI->x = left;
	windowDPI->y = top;
	windowDPI->width = right - left;
	windowDPI->height = bottom - top;
	windowDPI->pitch = screenDPI->width + screenDPI->pitch + left - right;

	for (w = RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window); w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++) {
		if (w->flags & WF_TRANSPARENT)
			continue;
		if (right <= w->x || bottom <= w->y)
			continue;
		if (left >= w->x + w->width || top >= w->y + w->height)
			continue;
		window_draw(w, left, top, right, bottom);
	}
}

/**
 *  Return the width of the string in buffer
 *
 *  rct2: 0x006C2321
 * buffer (esi)
 */
int gfx_get_string_width(char* buffer)
{
	// Current font sprites
	uint16* current_font_sprite_base;
	// Width of string
	int width;
	rct_g1_element* g1_element;
	
	current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	width = 0;

	for (uint8* curr_char = buffer; *curr_char != NULL; curr_char++) {

		if (*curr_char >= 0x20) {
			//Maybe global not address??
			width += RCT2_ADDRESS(0x0141E9E8, uint8)[*current_font_sprite_base + (*curr_char-0x20)];
			continue;
		}
		switch(*curr_char) {
		case 1:
			width = *curr_char;
			curr_char++;
			break;
		case 2:
		case 3:
		case 4:
			curr_char++;
			break;
		case 7:
			*current_font_sprite_base = 0x1C0;
			break;
		case 8:
			*current_font_sprite_base = 0x2A0;
			break;
		case 9:
			*current_font_sprite_base = 0x0E0;
			break;
		case 0x0A:
			*current_font_sprite_base = 0;
			break;
		case 0x17:
			g1_element = &(RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[*curr_char&0x7FFFF]);
			width += g1_element->width; //RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS + 4, uint16)[(*curr_char & 0x7FFFF) << 4];
			curr_char += 4;
			break;
		default:
			if (*curr_char <= 0x10) {
				continue;
			}
			curr_char += 2;
			if (*curr_char <= 0x16) {
				continue;
			}
			curr_char += 2;
			break;
		}
	}
	return width;
}

/**
 *  Clip the text in buffer to width, add ellipsis and return the new width of the clipped string
 *
 *  rct2: 0x006C2460
 * buffer (esi)
 * width (edi)
 */
int gfx_clip_string(char* buffer, int width)
{
	// Location of font sprites
	uint16* current_font_sprite_base;
	// Width the string has to fit into
	unsigned int max_width;
	// Character to change to ellipsis
	unsigned char* last_char;
	// Width of the string, including ellipsis
	unsigned int clipped_width;

	if (width < 6) {
		*buffer = 0;
		return 0;
	}
	
	current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	max_width = (uint32)width - (3 * RCT2_ADDRESS(0x141E9F6, uint8)[*current_font_sprite_base]);

	clipped_width = 0;
	last_char = buffer;

	for (unsigned char* curr_char = buffer; *curr_char != NULL; curr_char++) {
		if (*curr_char < 0x20) {
			switch(*curr_char) {
			case 1:
				width = *curr_char;
				curr_char++;
				continue;
			case 2:
			case 3:
			case 4:
				curr_char++;
				continue;
			case 7:
				*current_font_sprite_base = 0x1C0;
				break;
			case 8:
				*current_font_sprite_base = 0x2A0;
				break;
			case 9:
				*current_font_sprite_base = 0x0E0;
				break;
			case 0x0A:
				*current_font_sprite_base = 0;
				break;
			case 0x17:
				clipped_width += RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS + 4, uint16)[(*curr_char & 0x7FFFF) << 4];
				curr_char += 4;
				continue;
			default:
				if (*curr_char <= 0x10) {
					continue;
				}
				curr_char += 2;
				if (*curr_char <= 0x16) {
					continue;
				}
				curr_char += 2;
				continue;
			}
			max_width = width - (3 * RCT2_ADDRESS(0x141E9F6, uint8)[*current_font_sprite_base]);
		}

		clipped_width += RCT2_ADDRESS(0x0141E9E8, uint8)[*current_font_sprite_base + (*curr_char-0x20)];

		if (clipped_width >= width) {
			*((uint32*)last_char) = '...';
			clipped_width = width;
			return clipped_width;
		}
		if (clipped_width <= max_width) {
			last_char = curr_char;
		}
	}
	return clipped_width;
}

/**
 *  Wrap the text in buffer to width, returns width of longest line.
 *
 *  Inserts NULL where line should break (as \n is used for something else),
 *  so the number of lines is returned in num_lines. font_height seems to be
 *  a control character for line height.
 *
 *  rct2: 0x006C21E2
 * buffer (esi)
 * width (edi) - in
 * num_lines (edi) - out
 * font_height (ebx) - out
 */
int gfx_wrap_string(char* buffer, int width, int* num_lines, int* font_height)
{
	unsigned int line_width = 0;
	unsigned int max_width = 0;
	rct_g1_element* g1_element;

    uint16* current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);

    *num_lines = 0;

	// Pointer to the start of the current word
	unsigned char* curr_word = NULL;
	// Width of line up to current word
	unsigned int curr_width;

	for (unsigned char* curr_char = buffer; *curr_char != NULL; curr_char++) {

		// Remember start of current word and line width up to this word
        if (*curr_char == ' ') {
            curr_word = curr_char;
            curr_width = line_width;
        }

		// 5 is RCT2 new line?
        if (*curr_char != 5) {
			if (*curr_char < ' ') {
				switch(*curr_char) {
				case 1:
				case 2:
				case 3:
				case 4:
					curr_char++;
					continue;
				case 7:
					*font_height = 0x1C0;
					continue;
				case 8:
					*font_height = 0x2A0;
					continue;
				case 9:
					*font_height = 0xE0;
					continue;
				case 0x0A:
					*font_height = 0;
					continue;
				case 0x17:
					line_width += RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS + 4, uint16)[(*curr_char & 0x7FFFF)];
					curr_char += 4;
					break;
				default:
					if (*curr_char < 0x10) {
						continue;
					}
					curr_char += 2;
					if (*curr_char <= 0x16) {
						continue;
					}
					curr_char += 2;
					continue;
				}
			}

			line_width += RCT2_ADDRESS(0x0141E9E8, uint8)[*current_font_sprite_base + (*curr_char-0x20)];

			if (line_width <= width) {
				continue;
			}
			if (curr_word == 0) {
				curr_char--;
				unsigned char* old_char = curr_char;
				unsigned char swap_char = 0;
				unsigned char temp;
				// Insert NULL at current character
				// Aboslutely no guarantee that this won't overrun!
				do {
					temp = swap_char;
					swap_char = *curr_char;
					*curr_char = temp;
					curr_char++;
				} while(swap_char != 0);

				*curr_char = swap_char;
				curr_char = old_char;
				curr_char++;
				*num_lines += 1;

				if (line_width > max_width) {
					max_width = line_width;
				}
				line_width = 0;
				curr_word = 0;
				continue;
			}
			curr_char = curr_word;
			line_width = curr_width;
        }

        *num_lines += 1;
        *curr_char = 0;

		if (line_width > max_width) {
			max_width = line_width;
		}
		line_width = 0;
		curr_word = 0;
	}

	return max_width;
}


/**
 * Draws i formatted text string left aligned at i specified position but clips
 * the text with an elipsis if the text width exceeds the specified width.
 *  rct2: 0x006C1B83
 * dpi (edi)
 * format (bx)
 * args (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 * width (bp)
 */
void gfx_draw_string_left_clipped(rct_drawpixelinfo* dpi, int format, void* args, int colour, int x, int y, int width)
{
	char* buffer;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 0xE0;

	// Clip text - return value is not needed
	gfx_clip_string(buffer, width);

	gfx_draw_string(dpi, buffer, colour, x, y);
}

/**
 * Draws i formatted text string centred at i specified position but clips the
 * text with an elipsis if the text width exceeds the specified width.
 *  rct2: 0x006C1BBA
 * dpi (edi)
 * format (bx)
 * args (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 * width (bp)
 */
void gfx_draw_string_centred_clipped(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y, int width)
{
	char* buffer;
	short text_width;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 0xE0;

	// Clip text
	text_width = gfx_clip_string(buffer, width);

	// Draw the text centred
	if (text_width <= 0xFFFF) {
		x -= (text_width - 1) / 2;
		gfx_draw_string(dpi, buffer, colour, x, y);
	}
}

/**
 * Draws i formatted text string right aligned.
 *  rct2: 0x006C1BFC
 * dpi (edi)
 * format (bx)
 * args (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 */
void gfx_draw_string_right(rct_drawpixelinfo* dpi, int format, void* args, int colour, int x, int y)
{
	char* buffer;
	short text_width;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);

	// Measure text width
	text_width = gfx_get_string_width(buffer);

	// Draw the text right aligned
	x -= text_width;
	gfx_draw_string(dpi, buffer, colour, x, y);
}

/**
 * 
 *  rct2: 0x006C1E53
 * dpi (edi)
 * args (esi)
 * x (cx)
 * y (dx)
 * width (bp)
 * colour (al)
 * format (ebx)
 */
int gfx_draw_string_centred_wrapped(rct_drawpixelinfo *dpi, void *args, int x, int y, int width, int format, int colour)
{
	int font_height, line_width, line_y, num_lines;
	// Location of font sprites
	uint16* current_font_sprite_base;
	// Location of font flags
	uint16 current_font_flags;

	char* buffer = RCT2_ADDRESS(0x009C383D, char);

	current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	*current_font_sprite_base = 0xE0;

	gfx_draw_string(dpi, buffer, colour, dpi->x, dpi->y);

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);

	format_string(buffer, format, args);

	*current_font_sprite_base = 0xE0;

	// line_width unused here
	line_width = gfx_wrap_string(buffer, width, &num_lines, &font_height);

	RCT2_GLOBAL(0x00F43938, uint16) = 0x0A;

	if (font_height > 0xE0) {
		RCT2_GLOBAL(0x00F43938, uint16) = 6;
		if (font_height != 0x1C0) {
			RCT2_GLOBAL(0x00F43938, uint16) = 0x12;
		}
	}

	if (*buffer == 0x0B) {
		RCT2_GLOBAL(0x00F43938, uint16) = RCT2_GLOBAL(0x00F43938, uint16) + 1;
	}

	font_height = (RCT2_GLOBAL(0x00F43938, uint16) / 2) * num_lines;
	line_y = y - font_height;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;

	do {
		int half_width = gfx_get_string_width(buffer) / 2;
		gfx_draw_string(dpi, buffer, 0xFE, x - half_width, line_y);

		buffer += strlen(buffer) + 1;

        line_y += RCT2_GLOBAL(0x00F43938, uint16);
		num_lines--;
	} while (num_lines >= 0);

	return (sint16)(line_y & 0xFFFF) - y;
}

/**
 * 
 *  rct2: 0x006C2105
 * dpi (edi)
 * args (esi)
 * x (cx)
 * y (dx)
 * width (bp)
 * format (bx)
 * colour (al)
 */
int gfx_draw_string_left_wrapped(rct_drawpixelinfo *dpi, void *args, int x, int y, int width, int format, int colour)
{
	// font height might actually be something else
	int font_height, line_width, line_y, num_lines;
    
	// Location of font sprites
	uint16* current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	*current_font_sprite_base = 0xE0;

	char* buffer = RCT2_ADDRESS(0x009C383D, char);

	gfx_draw_string(dpi, buffer, colour, dpi->x, dpi->y);

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);

	format_string(buffer, format, args);

	*current_font_sprite_base = 0xE0;

	// Line width unused here
	line_width = gfx_wrap_string(buffer, width, &num_lines, &font_height);
	num_lines &= 0xFFFF;

    // Font height?
	RCT2_GLOBAL(0x00F43938, uint16) = 0x0A;

	if (font_height > 0xE0) {
		RCT2_GLOBAL(0x00F43938, uint16) = 6;
		if (font_height != 0x1C0) {
			RCT2_GLOBAL(0x00F43938, uint16) = 0x12;
		}
	}

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;

	line_y = y;

	do {
		gfx_draw_string(dpi, buffer, 0xFE, x, line_y);
		buffer += strlen(buffer) + 1;

        line_y += RCT2_GLOBAL(0x00F43938, uint16);
		// Bug if this line is removed?!
		font_height -= 1;

		num_lines--;
	} while (num_lines >= 0);

	return (sint16)(line_y & 0xFFFF) - y;

}

/**
 * Draws i formatted text string.
 *  rct2: 0x006C1B2F
 * dpi (edi)
 * format (bx)
 * args (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 */
void gfx_draw_string_left(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y)
{
	char* buffer;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 0xE0;
	gfx_draw_string(dpi, buffer, colour, x, y);
}


void colour_char(int al, uint16* current_font_flags) {

	int eax;

	rct_g1_element* g1_element = &(RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[0x1332]);
	eax = ((uint32*)g1_element->offset)[al & 0xFF];

	if (!(*current_font_flags & 2)) {
		eax = eax & 0x0FF0000FF;
	}
	// Adjust text palette. Store current colour? 
	RCT2_GLOBAL(0x009ABE05, uint32) = eax;
	RCT2_GLOBAL(0x009ABDA4, uint32) = 0x009ABE04;
}


void sub_682AC7(int ebp, uint16* current_font_flags) {

	int eax;

	 // loc_682AC7
	eax = RCT2_ADDRESS(0x0141FD45, uint8)[ebp * 8];
	if (*current_font_flags & 2) {
		eax |= 0x0A0A00;
	}
	 //Adjust text palette. Store current colour? 
	RCT2_GLOBAL(0x009ABE05, uint32) = eax;
	RCT2_GLOBAL(0x009ABDA4, uint32) = 0x009ABE04;
	eax = 0;
	// jmp     loc_682842

}


/**
 * 
 *  rct2: 0x00682702
 * dpi (edi)
 * buffer (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 */
void gfx_draw_string(rct_drawpixelinfo *dpi, char *buffer, int colour, int x, int y)
{
	//int eax, ebx, ecx, edx, esi, edi, ebp;
	//char* find = "FINDMEDRAWSTRING";
	//eax = colour;
	//ebx = 0;
	//ecx = x;
	//edx = y;
	//esi = (int)buffer;
	//edi = (int)dpi;
	//ebp = 0;
	//RCT2_CALLFUNC_X(0x00682702, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	//gLastDrawStringX = ecx;
	//gLastDrawStringY = edx;
	//
	int eax, ebx, ecx, edx, esi, edi, ebp;
	rct_g1_element* g1_element;

	// Maximum length/height of string
	int max_x = x;
	int max_y = y;

	// Store original x, y
	RCT2_GLOBAL(0x00EDF840, uint16) = x;
	RCT2_GLOBAL(0x00EDF842, uint16) = y;

	// 
	uint16* current_font_flags = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16);
	uint16* current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);

	// Flag for skipping non-printing characters
	int skip_char = 0;

	if (colour != 0xFE) {

		if (x >= dpi->x + dpi->width)
			return;
		
		if (x + 0x280 <= dpi->x)
			return;

		if (y >= dpi->y + dpi->height)
			return;

		if (y + 0x5A <= dpi->y) {
			return;
		}

		if (colour != 0xFF) {

			// switch_colour:
			*current_font_flags = 0;
			if (*current_font_sprite_base < 0) {
				*current_font_flags |= 4;
				if (*current_font_sprite_base != 0xFFFF) {
					*current_font_flags |= 8;
				}
				*current_font_sprite_base = 0xE0;
			}
			if (colour & (1 << 5)) {
				*current_font_flags |= 2;
			}
			colour &= ~(1 << 5);

			if (!(colour & 0x40)) {
				ebp = colour;
				if (*current_font_flags & 1) {
					if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
						skip_char = 1;
					} else {
						skip_char = 0;
					}
				} else {
					sub_682AC7(ebp, current_font_flags);
				}
			} else {
				*current_font_flags |= 1;
				colour &= 0x1F;

				if (*current_font_flags & 4) {
					if (*current_font_flags & 8) {
						eax = RCT2_ADDRESS(0x0141FC48, uint8)[colour * 8];
						eax = eax << 10;
						eax = eax | RCT2_ADDRESS(0x0141FC46, uint8)[colour * 8];
					} else {
						eax = RCT2_ADDRESS(0x0141FC49, uint8)[colour * 8];
						eax = eax << 10;
						eax = eax | RCT2_ADDRESS(0x0141FC47, uint8)[colour * 8];
					}
				} else {
					eax = RCT2_ADDRESS(0x0141FC4A, uint8)[colour * 8];
					eax = eax << 10;
					eax = eax | RCT2_ADDRESS(0x0141FC48, uint8)[colour * 8];
				}
				// Adjust text palette. Store current colour? ;
				RCT2_GLOBAL(0x009ABE05, uint32) = eax;
				RCT2_GLOBAL(0x009ABDA4, uint32) = 0x009ABE04;
				eax = 0;
			}
		}
	}

	if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
		skip_char = 1;
	}

	for (uint8 al = *buffer; al > 0; ++buffer, al = *buffer) {

		// Skip to the next printing character
		if (skip_char) {
			if (al < 0x20) {
				// Control codes
				skip_char = 0;
			} else if (al >= 0x8E && al < 0x9C) {
				// Colour codes
				al -= 0x8E;
				if (*current_font_flags == 1) {
					if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
						skip_char = 1;
					} else {
						skip_char = 0;
					}
					continue;
				}
				colour_char(al, current_font_flags);
				continue;
			} else {
				continue;
			}
		}

		if ((al >= 0x8E) && (al < 0x9C)){
			// Colour codes
			al -= 0x8E;
			if (*current_font_flags == 1) {
				if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
					skip_char = 1;
				} else {
					skip_char = 0;
				}
				continue;
			}
			colour_char(al, current_font_flags);
			continue;
		} else  {
			if (al < 0x20) {
				// Control codes
				al -= 0x20;
				switch (al) {
				case 0x0E5://Start New Line at set y lower
					max_x = x;//RCT2_GLOBAL(0x0EDF840, uint16);
					max_y += 0x0A;
					if (*current_font_sprite_base <= 0x0E) {
						break;
					}
					max_y -= 4;
					if (*current_font_sprite_base == 0x1C0) {
						break;
					}
					max_y -= 0xFFF4;
					break;
				case 0x0E6://Start New Line at set y lower
					max_x = x;//RCT2_GLOBAL(0x0EDF840, uint16);
					max_y += 5;
					if (*current_font_sprite_base <= 0x0E) {
						break;
					}
					max_y -= 2;
					if (*current_font_sprite_base == 0x1C0) {
						break;
					}
					max_y -= 0xFFFA;//This does not look correct probably should be an add
					break;
				case 0x0E1://Start New Line at start+buffer x, same y. (Overwrite?)
					al = *buffer;
					buffer++;
					max_x = x;//RCT2_GLOBAL(0x0EDF840, uint16);
					max_x += al;
					break;
				case 0x0F1: //Start new line at specified x,y
					eax = *((uint16*)buffer);
					buffer += 2;
					max_x = x;//RCT2_GLOBAL(0x0EDF840, uint16);
					max_x += (eax & 0xFF);
					max_y = y;//RCT2_GLOBAL(0x0EDF842, uint16);
					max_y += (eax & 0xFF00) >> 8;
					break;
				case 0x0E7:
					*current_font_sprite_base = 0x1C0;
					break;
				case 0x0E8:
					*current_font_sprite_base = 0x2A0;
					break;
				case 0x0E9:
					*current_font_sprite_base = 0xE0;
					break;
				case 0x0EA:
					*current_font_sprite_base = 0;
					break;
				case 0x0EB:
					*current_font_flags |= 2;
					break;
				case 0x0EC:
					*current_font_flags &= 0x0FFFD;
					break;
				case 0x0ED:
					ebp = RCT2_GLOBAL(0x0141F740, uint8);
					if (*current_font_flags & 1) {
						if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
							skip_char = 1;
						} else {
							skip_char = 0;
						}
						break;
					}
					sub_682AC7(ebp, current_font_flags);
					break;
				case 0x0EE:
					ebp = RCT2_GLOBAL(0x0141F741, uint8);	
					if (*current_font_flags & 1) {
						if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
							skip_char = 1;
						} else {
							skip_char = 0;
						}
						break;
					}
					sub_682AC7(ebp, current_font_flags);
					break;
				case 0x0EF:
					ebp = RCT2_GLOBAL(0x0141F742, uint8);
					if (*current_font_flags & 1) {
						if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
							skip_char = 1;
						} else {
							skip_char = 0;
						}
						break;
					}
					sub_682AC7(ebp, current_font_flags);
					break;													   
				case 0x0E2:
					al = *buffer;
					buffer++;
					if (*current_font_flags & 1) {
						if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
							skip_char = 1;
							break;
						}
					}
		
					eax = RCT2_ADDRESS(0x097FCBC, uint32)[al*4];
					g1_element = &(RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[eax]);
					ebx = g1_element->offset[0xF9] + (1 << 8);

					if (!(*current_font_flags & 2)) {
						ebx = ebx & 0xFF;
					}
					//Adjust the text palette
					RCT2_GLOBAL(0x09ABE05, uint16) = ebx;
					memcpy((uint8*)0x09ABE07,&(g1_element->offset[0xF7]),2);
					//ebx = g1_element->offset[0xF7];
					//RCT2_GLOBAL(0x09ABE07, uint16) = ebx;
					memcpy((uint8*)0x09ABE09,&(g1_element->offset[0xFA]),2);
					//ebx = g1_element->offset[0xFA];
					//RCT2_GLOBAL(0x09ABE09, uint16) = ebx;
					//Set the palette pointer
					RCT2_GLOBAL(0x09ABDA4, uint32) = 0x09ABE04;
					if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
						skip_char = 1;
					}
					break;
				case 0x0F7:
					buffer += 5;
					if (max_x >= dpi->x + dpi->width) {
						skip_char = 1;
						break;
					}
					ebx = *((uint32*)(buffer - 4));
					eax = ebx & 0x7FFFF;
					g1_element = &(RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[eax]);
		
					gfx_draw_sprite(dpi, ebx, max_x, max_y);

					max_x = max_x + g1_element->width;
					break;
				}

			} else {
				al -= 0x20;
				if (max_x >= dpi->x + dpi->width) {
					skip_char = 1;
				}
				if (max_x + 0x1A < dpi->x) {
					ebx = al;
					ebx += *current_font_sprite_base;
					max_x = max_x + (RCT2_ADDRESS(0x0141E9E8, uint8)[ebx] & 0xFF);
					continue;
				}
				ebx = al + *current_font_sprite_base;

				ecx = max_x;

				max_x = max_x + (RCT2_ADDRESS(0x0141E9E8, uint8)[ebx] & 0xFF);

				eax = (int)al;
				ebx += 0xF15;

				edx = max_y;
				esi = (int)buffer;
				edi = (int)dpi;
				ebp = 0;

				RCT2_GLOBAL(0x00EDF81C, uint32) = 0x20000000;
				
				gfx_draw_sprite_palette_set(dpi, 0x20000000 | ebx, ecx, edx, RCT2_GLOBAL(0x9ABDA4, uint8*), NULL);

				continue;
			}
		} 
	}

	gLastDrawStringX = max_x;
	gLastDrawStringY = max_y;
}
