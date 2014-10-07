/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill, Duncan Frost
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

#include "../addresses.h"
#include "../common.h"
#include "drawing.h"

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

	uint16 cross_pattern = 0;

	int start_x = left - dpi->x;
	if (start_x < 0){
		start_x = 0;
		cross_pattern ^= start_x;
	}

	int end_x = right - dpi->x;
	end_x++;
	if (end_x > dpi->width)
		end_x = dpi->width;

	int width = end_x - start_x;

	int start_y = top - dpi->y;
	if (start_y < 0){
		start_y = 0;
		cross_pattern ^= start_y;
	}
	int end_y = bottom - dpi->y;
	end_y++;

	if (end_y > dpi->height)
		end_y = dpi->height;

	int height = end_y - start_y;
	if (colour&0x1000000){
		// 00678B2E    00678BE5
		//Cross hatching
		uint8* dest_pointer = (start_y * (dpi->width + dpi->pitch)) + start_x + dpi->bits;
	
		uint32 ecx;
		for (int i = 0; i < height; ++i) {
			uint8* next_dest_pointer = dest_pointer + dpi->width + dpi->pitch;
			ecx = cross_pattern;
			// Rotate right
			ecx = (ecx >> 1) | (ecx << (sizeof(ecx) * CHAR_BIT - 1));
			ecx = (ecx & 0xFFFF0000) | width; 
			// Fill every other pixel with the colour
			for (; (ecx & 0xFFFF) > 0; ecx--) {
				ecx = ecx ^ 0x80000000;
				if ((int)ecx < 0) {
					*dest_pointer = colour & 0xFF;
				}
				dest_pointer++;
			}
			cross_pattern ^= 1;
			dest_pointer = next_dest_pointer;
			
		}
		return;
	}
	if (colour & 0x2000000){
		//0x2000000
		// 00678B7E   00678C83
		// Location in screen buffer?
		uint8* dest_pointer = dpi->bits + (uint32)((start_y >> (dpi->zoom_level)) * ((dpi->width >> dpi->zoom_level) + dpi->pitch) + (start_x >> dpi->zoom_level));

		// Find colour in colour table?
		uint16 eax = palette_to_g1_offset[(colour & 0xFF)];
		rct_g1_element g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[eax];

		// Fill the rectangle with the colours from the colour table
		for (int i = 0; i < height>>dpi->zoom_level; ++i) {
			uint8* next_dest_pointer = dest_pointer + (dpi->width >> dpi->zoom_level) + dpi->pitch;
			for (int j = 0; j < width; ++j) {
				*dest_pointer = g1_element.offset[*dest_pointer];
				dest_pointer++;
			}
			dest_pointer = next_dest_pointer;
		}
		return;
	}
	if (colour & 0x4000000){
		//0x4000000
		// 00678B8A   00678E38
		char* dest_pointer;
		dest_pointer = start_y * (dpi->width + dpi->pitch) + start_x + dpi->bits;

		//The pattern loops every 15 lines this is which
		//part the pattern is on.
		int pattern_y = (start_y + dpi->y) % 16;

		//The pattern loops every 15 pixels this is which
		//part the pattern is on.
		int start_pattern_x = (start_x + dpi_->x) % 16;
		int pattern_x = start_pattern_x;

		uint16* pattern_pointer;
		pattern_pointer = RCT2_ADDRESS(0x0097FEFC,uint16*)[colour >> 28]; // or possibly uint8)[esi*4] ?

		for (int no_lines = height; no_lines > 0; no_lines--) {
			char* next_dest_pointer = dest_pointer + dpi->width + dpi->pitch;
			uint16 pattern = pattern_pointer[pattern_y]; 

			for (int no_pixels = width; no_pixels > 0; --no_pixels) {
				if (pattern & (1 << pattern_x))
					*dest_pointer = colour & 0xFF;

				pattern_x = (pattern_x + 1) % 16;
				dest_pointer++;
			}
			pattern_x = start_pattern_x;
			pattern_y = (pattern_y + 1) % 16;
			dest_pointer = next_dest_pointer;
		}
		return;
	}
	if (colour & 0x8000000){
		//0x8000000
		// 00678B3A    00678EC9 still to be implemented
		//RCT2_CALLPROC_X(0x00678AD4, left, right, top, bottom, 0, dpi, colour);
		int esi = left - RCT2_GLOBAL(0x1420070,sint16);
		RCT2_GLOBAL(0xEDF824,uint32) = esi;
		esi = top - RCT2_GLOBAL(0x1420072,sint16);
		RCT2_GLOBAL(0xEDF828,uint32) = esi;
		left -= dpi->x;//0x4
		if ( left < 0 ){
			RCT2_GLOBAL(0xEDF824,sint32) -= left;
			left = 0; 
		}
		right -= dpi->x;
		right++;
		if ( right > dpi->width ){
			right = dpi->width;
		}
		right -= left;
		top -= dpi->y;
		if ( top < 0 ){
			RCT2_GLOBAL(0xEDF828,sint32) -= top;
			top = 0;
		}
		bottom -= dpi->y;
		bottom++;
		if (bottom > dpi->height){
			bottom = dpi->height;
		}
		bottom -= top;
		RCT2_GLOBAL(0xEDF824,sint32) &= 0x3F;
		RCT2_GLOBAL(0xEDF828,sint32) &= 0x3F;
		esi = dpi->width;
		esi += dpi->pitch;
		esi *= top;
		esi += left;
		esi += (uint32)dpi->bits;
		RCT2_GLOBAL(0xEDF82C,sint32) = right;
		RCT2_GLOBAL(0xEDF830,sint32) = bottom;
		left = dpi->width;
		left+= dpi->pitch;
		left-= right;
		RCT2_GLOBAL(0xEDF834,sint32) = left;
		colour &= 0xFF;
		colour--;
		right = colour;
		colour <<= 8;
		right |= colour;
		RCT2_GLOBAL(0xEDF838,sint32) = right;
		//right <<= 4;
		int edi = esi;
		esi = RCT2_GLOBAL(0xEDF828,sint32);
		esi *= 0x40;
		left = 0;
		esi += (uint32)(RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS,rct_g1_element)[right]).offset;//???
		//Not finished
		//Start of loop
		return;
	}
	//0x0000000
	uint8* dest_pointer = start_y * (dpi->width + dpi->pitch) + start_x + dpi->bits;

	for (int i = 0; i < height; ++i) {
		memset(dest_pointer, (colour & 0xFF), width);
		dest_pointer += dpi->width + dpi->pitch;
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