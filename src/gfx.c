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

#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include "addresses.h"
#include "gfx.h"
#include "rct2.h"
#include "strings.h"
#include "window.h"
#include "osinterface.h"

// HACK These were originally passed back through registers
int gLastDrawStringX;
int gLastDrawStringY;

uint8 _screenDirtyBlocks[5120];

static void gfx_draw_dirty_blocks(int x, int y, int columns, int rows);

/**
 * 
 *  rct2: 0x00678998
 */
void gfx_load_g1()
{
	HANDLE hFile;
	DWORD bytesRead;
	DWORD header[2];

	int i;
	int g1BufferSize;
	void* g1Buffer;

	rct_g1_element *g1Elements = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element);

	hFile = CreateFile(get_file_path(PATH_ID_G1), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		ReadFile(hFile, header, 8, &bytesRead, NULL);
		if (bytesRead == 8) {
			g1BufferSize = header[1];
			g1Buffer = rct2_malloc(g1BufferSize);
			ReadFile(hFile, g1Elements, 29294 * sizeof(rct_g1_element), &bytesRead, NULL);
			ReadFile(hFile, g1Buffer, g1BufferSize, &bytesRead, NULL);
			CloseHandle(hFile);

			for (i = 0; i < 29294; i++)
				g1Elements[i].offset += (int)g1Buffer;

			return;
		}
	}

	// exit with error
	fprintf(stderr, "Unable to load g1.dat");
	assert(0);
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
	memset(bits_pointer, colour, no_pixels);
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
	RCT2_CALLPROC_X(0x00678AD4, left, right, top, bottom, 0, (int)dpi, colour);
}

/**
 * 
 *  rct2: 0x006E6F81
 * dpi (edi)
 * left (ax)
 * top (cx)
 * right (bx)
 * bottom (dx)
 * colour (ebp)
 * _si (si)
 */
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, short left, short top, short right, short bottom, int colour, short _si)
{
	RCT2_CALLPROC_X(0x006E6F81, left, right, top, bottom, _si, (int)dpi, colour);
}

#define RCT2_Y_RELATED_GLOBAL_1 0x9E3D12 //uint16
#define RCT2_Y_END_POINT_GLOBAL 0x9ABDAC //sint16
#define RCT2_Y_START_POINT_GLOBAL 0xEDF808 //sint16
#define RCT2_X_RELATED_GLOBAL_1 0x9E3D10 //uint16
#define RCT2_X_END_POINT_GLOBAL 0x9ABDA8 //sint16
#define RCT2_X_START_POINT_GLOBAL 0xEDF80C //sint16
#define RCT2_DPI_LINE_LENGTH_GLOBAL 0x9ABDB0 //uint16 width+pitch

/*
* rct2: 0x67A690 very similar in function to 0x67AA18 readied images are copied onto
* buffers
*/
void sub_0x67A690(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp, int image_type){

	//Has a background image?
	if (image_type & 0x2){
		int _ecx = eax >> 8;
		uint32 _ebx = RCT2_GLOBAL(0x9ABDA4, uint32);
		_ecx--;
		_ecx <<= 0x10;

		//Mix with background image and colour adjusted
		if (RCT2_GLOBAL(0x9E3CDC, uint32)){
			RCT2_GLOBAL(0x9E3D04, uint32) = ebp;
			uint32 _ebp = RCT2_GLOBAL(0x9E3CDC, uint32);
			_ebp += RCT2_GLOBAL(0x9E3CE0, uint32);

			for (_ecx >>= 0x10; _ecx >= 0; --_ecx){
				for (int _cx = RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, uint16); _cx > 0; --_cx){
					uint8 al = *((char*)esi);
					esi++;
					al = *((char*)_ebx + al);
					al &= *((char*)_ebp);
					if (al){
						*((char*)edi) = al;
					}
					edi++;
					_ebp++;
				}
				esi += edx;
				edi += RCT2_GLOBAL(0x9E3D04, uint32);
				_ebp += edx;
			}
			return;
		}

		//Quicker?
		if ((RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, uint16)) == 4){
			
			ebp += 4;
			edx += 4;
			for (_ecx >>= 0x10; _ecx >= 0; --_ecx){
				for (int i = 0; i < 4; ++i){
					uint8 al = *((char*)esi+i);
					al = *((char*)_ebx + al);
					if (al){
						*((char*)edi+i) = al;
					}
				}
				edi += ebp;
				esi += edx;
			}
			return;
		}

		//image colour adjusted?
		for (_ecx >>= 0x10; _ecx >= 0; --_ecx){
			for (int _cx = RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, uint16); _cx > 0; --_cx){
				uint8 al = *((char*)esi);
				esi++;
				al = *((char*)_ebx + al);
				if (al){
					*((char*)edi) = al;
				}
				edi++;
			}
			esi += edx;
			edi += ebp;
		}
		return;
	}

	//image_type mix with background?
	if (image_type & 0x4){
		int _ebx = RCT2_GLOBAL(0x9ABDA4, uint16);
		for (int ah = eax >> 8; ah > 0; --ah){
			
			for (int cx = RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, uint16); cx > 0; --ecx){
				uint8 al = *((char*)esi);
				esi++;
				if (al){
					al = *((char*)edi);
					al = *((char*)_ebx + al);
					*((char*)edi) = al;
				}
				edi++;
			}
			esi += edx;
			edi += ebp;
		}
		return;
	}

	if (!(RCT2_GLOBAL(0x9E3D14, uint16) & 1)){
		int bx = RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, uint16);
		for (int ax = RCT2_GLOBAL(0x9ABDAC, uint16); ax > 0; --ax){
			memcpy((char*)edi, (char*)esi, bx);
			edi += bx;
			esi += bx;
			edi += ebp;
			esi += edx;
		}
		return;
	}

	int _ebx = RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, uint16);
	if (RCT2_GLOBAL(0x9E3CDC, uint32) != 0){
		RCT2_GLOBAL(0x9E3D04, uint32) = ebp;
		uint32 _ebp = RCT2_GLOBAL(0x9E3CDC, uint32);
		_ebp += RCT2_GLOBAL(0x9E3CE0, uint32);

		for (int ah = eax >> 8; ah > 0; --ah){
			for (int _ecx = _ebx; _ecx > 0; --_ecx){
				char al = *((char*)esi);
				esi++;
				al &= *((char*)_ebp);
				if (al){
					*((char*)edi) = al;
				}
				edi++;
				_ebp++;
			}
			esi += edx;
			edi += RCT2_GLOBAL(0x9E3D04, uint32);
			_ebp += edx;
		}
	}

	for (int ah = eax >> 8; ah > 0; --ah){
		for (int _ecx = _ebx; _ecx > 0; --_ecx){
			char al = *((char*)esi);
			esi++;
			if (al){
				*((char*)edi) = al;
			}
			edi++;
		}
		esi += edx;
		edi += ebp;
	}
	return;
}

/*
* rct2: 0x67AA18 transfers readied images onto buffers
* This function copies the sprite data onto the screen
* I think its only used for bitmaps onto buttons but i am not sure.
* There is still a small bug with this code when it is in the choose park view.
*/
void sub_0x67AA18(char* source_bits_pointer, char* dest_bits_pointer, rct_drawpixelinfo *dpi, int image_type, int g1_y_start, int g1_y_end, int g1_x_start, int g1_x_end){
	//Image_id
	if (image_type & 0x2){

		RCT2_GLOBAL(RCT2_DPI_LINE_LENGTH_GLOBAL, uint16) = dpi->width + dpi->pitch;
		RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, sint16) = g1_x_end;
		RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, sint16) = g1_y_end;
		RCT2_GLOBAL(RCT2_Y_START_POINT_GLOBAL, uint16) = g1_y_start;
		RCT2_GLOBAL(RCT2_X_START_POINT_GLOBAL, uint16) = g1_x_start;
		RCT2_CALLPROC_X_EBPSAFE(0x67AA18, 0, 0, 0, 0, (int)source_bits_pointer, (int)dest_bits_pointer, (int)dpi);
		return; //0x67AAB3
	}

	if (image_type & 0x4){
		RCT2_GLOBAL(RCT2_DPI_LINE_LENGTH_GLOBAL, uint16) = dpi->width + dpi->pitch;
		RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, sint16) = g1_x_end;
		RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, sint16) = g1_y_end;
		RCT2_GLOBAL(RCT2_Y_START_POINT_GLOBAL, uint16) = g1_y_start;
		RCT2_GLOBAL(RCT2_X_START_POINT_GLOBAL, uint16) = g1_x_start;
		RCT2_CALLPROC_X_EBPSAFE(0x67AA18, 0, 0, 0, 0, (int)source_bits_pointer, (int)dest_bits_pointer, (int)dpi);
		return; //0x67AFD8
	}
	
	uint16 offset_to_first_line = *(uint16*)(g1_y_start*2 + (uint32)source_bits_pointer);
	//This will now point to the first line
	char* next_source_pointer = (char*)((uint32)source_bits_pointer + offset_to_first_line);
	char* next_dest_pointer = dest_bits_pointer;
	
	//For every line in the image
	for (; g1_y_end; g1_y_end--){

		uint8 last_data_line = 0;
		//For every data section in the line
		while (!last_data_line){
			char* source_pointer = next_source_pointer;
			char* dest_pointer = next_dest_pointer;

			int no_pixels = *source_pointer++;
			uint8 gap_size = *source_pointer++;
			//The last bit in no_pixels tells you if you have reached the end of a line
			last_data_line = no_pixels & 0x80;
			//Clear the last data line bit so we have just the no_pixels
			no_pixels &= 0x7f;
			//Have our next source pointer point to the next data section
			next_source_pointer = source_pointer + no_pixels;

			//Calculates the start point of the image
			int x_start = gap_size - g1_x_start;

			if (x_start > 0){
				//Since the start is positive
				//We need to move the drawing surface to the correct position
				dest_pointer += x_start;
			}
			else{
				//If the start is negative we require to remove part of the image.
				//This is done by moving the image pointer to the correct position.
				source_pointer -= x_start;
				//The no_pixels will be reduced in this operation
				no_pixels += x_start;
				//If there are no pixels there is nothing to draw this line
				if (no_pixels <= 0) continue;
				//Reset the start position to zero as we have taken into account all moves
				x_start = 0;
			}

			int x_end = x_start + no_pixels;
			//If the end position is further out than the whole image
			//end position then we need to shorten the line again
			if (x_end > (int)g1_x_end){
				//Shorten the line
				no_pixels -= x_end - g1_x_end;
				//If there are no pixels there is nothing to draw.
				if (no_pixels <= 0) continue;
			}

			//Finally after all those checks, copy the image onto the drawing surface
			memcpy(dest_pointer, source_pointer, no_pixels);
		}
		//Add a line to the drawing surface pointer
		next_dest_pointer += (int)dpi->width + (int)dpi->pitch;
	}
}


/*
* rct2: 0x67A934 title screen bitmaps on buttons
* This function readies all the global vars for copying the sprite data onto the screen
* I think its only used for bitmaps onto buttons but i am not sure.
*/
void sub_0x67A934(rct_g1_element *source_g1, rct_drawpixelinfo *dest_dpi, int x, int y, int image_type){
	int g1_y_start, g1_x_start;
	char* bits_pointer;

	bits_pointer = dest_dpi->bits;
	g1_y_start = 0;

	int start_y, end_y;
	start_y = y + source_g1->y_offset - dest_dpi->y;
	
	//If the start position is negative reset to zero
	if (start_y < 0){
		//Create the end point within the drawing area
		end_y = source_g1->height + start_y;
		//If the end point is now <= 0 no need to draw
		if (end_y <= 0)return;

		g1_y_start -= start_y;
		start_y = 0;
	}
	else{
		end_y = source_g1->height;
		//Move the pointer to the correct starting y location
		bits_pointer += (dest_dpi->width + dest_dpi->pitch)*start_y;
	}

	int height = start_y + end_y;
	//If the image is taller than the drawing area
	if (height > dest_dpi->height){
		//Make the end within the drawing area
		end_y -= height - dest_dpi->height;
		//If the end is now <=0 then there is nothing to draw
		if (end_y <= 0)return;
	}
	
	
	int start_x, end_x;
	g1_x_start = 0;
	start_x = x + source_g1->x_offset - dest_dpi->x;
	
	//If the start position is negative reset to zero
	if (start_x < 0){
		//Create the end point within the drawing area
		end_x = source_g1->width + start_x;
		//If the end point is now <= 0 no need to draw
		if (end_x <= 0)return;

		g1_x_start -= start_x;
		start_x = 0;
	}
	else{
		end_x = source_g1->width;
		//Increment the pointer to our start location
		bits_pointer += start_x;
	}

	int width = start_x + end_x;
	//If the image is wider than the drawing area
	if (width > dest_dpi->width){
		//Make the end within drawing area
		end_x -= width - dest_dpi->width;
		//If the end is now <=0 then there is nothing to draw
		if (end_x <= 0)return;
	}
	
	sub_0x67AA18((char*)source_g1->offset, bits_pointer, dest_dpi, image_type, g1_y_start, end_y, g1_x_start, end_x);
}

/**
 *
 *  rct2: 0x0067A28E
 * image_id (ebx)
 * x (cx)
 * y (dx)
 */
void gfx_draw_sprite(rct_drawpixelinfo *dpi, int image_id, int x, int y)
{
	//RCT2_CALLPROC_X(0x0067A28E, 0, image_id, x, y, 0, dpi, 0);
	//return;

	int eax = 0, ebx = image_id, ecx = x, edx = y, esi = 0, edi = (int)dpi, ebp = 0;
	int image_type = (image_id & 0xE0000000) >> 28;

	RCT2_GLOBAL(0x00EDF81C, uint32) = image_id & 0xE0000000;
	eax = (image_id >> 26) & 0x7;
	//eax = RCT2_GLOBAL(0x009E3CE4 + eax*4, uint32);
	RCT2_GLOBAL(0x009E3CDC, uint32) = RCT2_GLOBAL(0x009E3CE4 + eax * 4, uint32);

	if (((image_id)& 0xE0000000) && !(image_id & (1 << 31))) {

		if (!((image_id)& 0x40000000)){
			eax = image_id;
			eax >>= 19;
			eax &= 0xFF;
			RCT2_GLOBAL(0x009E3CDC, uint32) = 0;
		}
		else{
			eax = image_id;
			eax >>= 19;
			eax &= 0x7F;
		}
		eax = RCT2_GLOBAL(eax * 4 + 0x97FCBC, uint32);
		eax <<= 4;
		eax = RCT2_GLOBAL(eax + RCT2_ADDRESS_G1_ELEMENTS, uint32);
		RCT2_GLOBAL(0x9ABDA4, uint32) = eax;

	}
	else if (((image_id)& 0xE0000000) && !(image_id & (1 << 29))){
		//RCT2_CALLPROC_X(0x0067A28E, 0, image_id, x, y, 0,(int) dpi, 0);
		//return;//jump into 0x67a361

		eax = image_id;
		RCT2_GLOBAL(0x9E3CDC, uint32) = 0;
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
		image_type |= 0x2;

		eax = RCT2_GLOBAL(eax * 4 + 0x97FCBC, uint32);
		eax <<= 4;
		eax = RCT2_GLOBAL(eax + RCT2_ADDRESS_G1_ELEMENTS, uint32);
		edx = *((uint32*)(eax + 0xF3));
		esi = *((uint32*)(eax + 0xF7));
		RCT2_GLOBAL(0x9ABFD6, uint32) = edx;
		RCT2_GLOBAL(0x9ABFDA, uint32) = esi;
		edx = *((uint32*)(eax + 0xFB));
		RCT2_GLOBAL(0x9ABDA4, uint32) = 0x9ABF0C;
		RCT2_GLOBAL(0x9ABFDE, uint32) = edx;
		edx = y;

	}
	else if ((image_id)& 0xE0000000){
		//RCT2_CALLPROC_X(0x0067A28E, 0, image_id, x, y, 0,(int) dpi, 0);		
		//return;
		eax = image_id;
		RCT2_GLOBAL(0x9E3CDC, uint32) = 0;
		eax >>= 19;
		eax &= 0x1f;
		eax = RCT2_GLOBAL(eax * 4 + 0x97FCBC, uint32);
		eax <<= 4;
		eax = RCT2_GLOBAL(eax + RCT2_ADDRESS_G1_ELEMENTS, uint32);

		ebp = *((uint32*)(eax + 0xF3));
		esi = *((uint32*)(eax + 0xF7));
		RCT2_GLOBAL(0x9ABEFF, uint32) = ebp;
		RCT2_GLOBAL(0x9ABF03, uint32) = esi;
		ebp = *((uint32*)(eax + 0xFB));
		eax = ebx;

		RCT2_GLOBAL(0x9ABF07, uint32) = ebp;
		eax >>= 24;
		eax &= 0x1f;
		eax = RCT2_GLOBAL(eax * 4 + 0x97FCBC, uint32);
		eax <<= 4;
		eax = RCT2_GLOBAL(eax + RCT2_ADDRESS_G1_ELEMENTS, uint32);
		ebp = *((uint32*)(eax + 0xF3));
		esi = *((uint32*)(eax + 0xF7));
		RCT2_GLOBAL(0x9ABED6, uint32) = ebp;
		RCT2_GLOBAL(0x9ABEDA, uint32) = esi;
		ebp = *((uint32*)(eax + 0xFB));

		RCT2_GLOBAL(0x9ABDA4, uint32) = 0x009ABE0C;
		RCT2_GLOBAL(0x9ABEDE, uint32) = ebp;
		
	}

	ebx &= 0x7FFFF;
	ebx <<= 4;
	ebx += RCT2_ADDRESS_G1_ELEMENTS;
	if (dpi->pad_0E >= 1){
		if (dpi->pad_0E == 1){
			RCT2_CALLPROC_X(0x0067BD81, eax, ebx, x, y, 0,(int) dpi, ebp);
			return;
		}
		if (dpi->pad_0E >= 3){
			RCT2_CALLPROC_X(0x0067FAAE, eax, ebx, x, y, 0, (int)dpi, ebp);
			return;
		}
		RCT2_CALLPROC_X(0x0067DADA, eax, ebx, x, y, 0, (int)dpi, ebp);
		return;
	}
	eax = *((uint32*)ebx + 2);
	ebp = *((uint32*)ebx + 3);
	rct_g1_element* g1_source = (rct_g1_element*)ebx;
	//This is a rct2_drawpixelinfo struct
	RCT2_GLOBAL(0x9E3D08, uint32) = *((uint32*)ebx); //offset to g1 bits?
	RCT2_GLOBAL(0x9E3D0C, uint32) = *((uint32*)ebx + 1);
	RCT2_GLOBAL(0x9E3D10, uint32) = *((uint32*)ebx + 2); //X-Y related unsigned? sets RCT2_X_RELATED_GLOBAL_1 and Y
	RCT2_GLOBAL(0x9E3D14, uint32) = *((uint32*)ebx + 3);

	if (RCT2_GLOBAL(0x9E3D14, uint32) & (1 << 2)){
		//Title screen bitmaps
		sub_0x67A934(g1_source, dpi, x, y, image_type);
		return;
	}

	//dpi on stack
	int translated_x, translated_y;
	char* bits_pointer;

	ebp = (int)dpi;
	esi = (int)g1_source->offset;//RCT2_GLOBAL(0x9E3D08, uint32);
	RCT2_GLOBAL(0x9E3CE0, uint32) = 0;
	bits_pointer = dpi->bits;	

	RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, sint16) = g1_source->height;// RCT2_GLOBAL(0x9E3D0E, sint16);

	translated_y = y - dpi->y + g1_source->y_offset;//RCT2_GLOBAL(RCT2_Y_RELATED_GLOBAL_1, uint16);

	if (image_id == 5200){
		image_id = image_id;
	}

	if (translated_y < 0){
		RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, sint16) += translated_y;
		if (RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, sint16) <= 0){
			return;
		}
		translated_y = -translated_y;
		esi += (uint32)translated_y * g1_source->width;//RCT2_GLOBAL(0x9E3D0C, sint16);
		RCT2_GLOBAL(0x9E3CE0, sint32) += translated_y * g1_source->width;//RCT2_GLOBAL(0x9E3D0C, sint16);
		translated_y = 0;
	} else {
		//eax = ;
		bits_pointer += (dpi->width + dpi->pitch) * translated_y;//eax;
	}

	translated_y += RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, sint16) - dpi->height;

	if (translated_y > 0){
		RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, sint16) -= translated_y;
		if (RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, sint16) <= 0)return;
	}

	RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, sint16) = g1_source->width;// RCT2_GLOBAL(0x9E3D0C, sint16);
	eax = dpi->width + dpi->pitch - g1_source->width; //RCT2_GLOBAL(0x9E3D0C, sint16);

	RCT2_GLOBAL(0x9ABDAE, uint16) = 0;
	RCT2_GLOBAL(RCT2_DPI_LINE_LENGTH_GLOBAL, sint16) = dpi->width + dpi->pitch - g1_source->width;// RCT2_GLOBAL(0x9E3D0C, sint16);
	translated_x = x - dpi->x + g1_source->x_offset;//RCT2_GLOBAL(RCT2_X_RELATED_GLOBAL_1, uint16);

	if (translated_x < 0){

		RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, sint16) += translated_x;
		if (RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, sint16) <= 0){
			return;
		}

		RCT2_GLOBAL(0x9ABDAE, sint16) -= translated_x;
		esi -= translated_x;
		RCT2_GLOBAL(0x9E3CE0, sint32) -= translated_x;
		RCT2_GLOBAL(RCT2_DPI_LINE_LENGTH_GLOBAL, sint16) -= translated_x;
		translated_x = 0;
	}

	bits_pointer += translated_x;
	translated_x += RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, sint16);
	

	translated_x -= dpi->width;

	if (translated_x > 0){
		RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, sint16) -= translated_x;
		if (RCT2_GLOBAL(RCT2_X_END_POINT_GLOBAL, sint16) <= 0)return;

		RCT2_GLOBAL(0x9ABDAE, uint16) += translated_x;
		RCT2_GLOBAL(RCT2_DPI_LINE_LENGTH_GLOBAL, uint16) += translated_x;
	}
	
	if (!(g1_source->flags & 0x02)){
		eax = RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, uint8);
		eax <<= 8;
		edx = RCT2_GLOBAL(0x9ABDAE, uint16);
		ebp = RCT2_GLOBAL(RCT2_DPI_LINE_LENGTH_GLOBAL, uint16);
		ebx = RCT2_GLOBAL(0xEDF81C, uint32);
		ecx = 0xFFFF&translated_x;
		//ebx, edx, esi, edi, ah, ebp used in 0x67a690  eax=1966, ecx=ff39, edx=ebx=0, esp = cfca4, ebp = 266, esi =368823e, edi = 16c79b2
		sub_0x67A690(eax, ebx, ecx, edx, esi, (int)bits_pointer, ebp, image_type);
		return;
	}
	//0x67A60A
	
	esi -= (uint32)g1_source->offset;
	ebp = esi;
	eax = g1_source->width*g1_source->height;
	esi = (int)g1_source->offset;
	edx = eax;
	edi = 0x9E3D28;
	eax = 0;
	while (edx>0){
		eax = *((sint8*)esi);
		if (eax >= 0){
			esi++;
			ecx = eax;
			edx -= eax;
			memcpy((char*)edi, (char*)esi, ecx);
			edi += ecx;
			esi += ecx;
			continue;
		}
		ecx = eax;
		ebx = edi;
		eax &= 0x7;
		ecx >>= 3;
		eax <<= 8;
		ecx = -ecx;
		eax = eax & 0xFF00 + *((sint8*)esi);
		edx -= ecx;
		esi += 2;
		ebx -= eax;
		eax = esi;
		esi = ebx;
		ebx = eax;
		eax = 0;
		memcpy((char*)edi, (char*)esi, ecx);
		edi += ecx;
		esi += ecx;
		esi = ebx;
	}
	//edi poped off stack
	esi = ebp;
	esi += 0x9E3D28;
	eax = RCT2_GLOBAL(RCT2_Y_END_POINT_GLOBAL, uint8);
	eax <<= 8;
	edx = RCT2_GLOBAL(0x9ABDAE, uint16);
	ebp = RCT2_GLOBAL(RCT2_DPI_LINE_LENGTH_GLOBAL, uint16);
	ebx = RCT2_GLOBAL(0xEDF81C, uint32);

	sub_0x67A690(eax, ebx, ecx, edx, esi, (int)bits_pointer, ebp, image_type);
	//RCT2_CALLPROC_X(0x0067A28E, 0, image_id, x, y, 0, (int)dpi, 0);
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
	RCT2_CALLPROC_X(0x006C1D6C, colour, format, x, y, (int)args, (int)dpi, 0);
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
 * 
 *  rct2: 0x006C2321
 * buffer (esi)
 */
int gfx_get_string_width(char *buffer)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	esi = (int)buffer;
	RCT2_CALLFUNC_X(0x006C2321, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	return ecx & 0xFFFF;
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
	RCT2_CALLPROC_X(0x006C1B83, colour, format, x, y, (int)args, (int)dpi, width);

	//char* buffer;

	//buffer = (char*)0x0141ED68;
	//format_string(buffer, format, args);
	//rctmem->current_font_sprite_base = 224;
	//clip_text(buffer, width);
	//gfx_draw_string(dpi, buffer, colour, x, y);
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
	RCT2_CALLPROC_X(0x006C1BBA, colour, format, x, y, (int)args, (int)dpi, width);

	//char* buffer;
	//short text_width;

	//buffer = (char*)0x0141ED68;
	//format_string(buffer, format, args);
	//rctmem->current_font_sprite_base = 224;
	//text_width = clip_text(buffer, width);

	//// Draw the text centred
	//x -= (text_width - 1) / 2;
	//gfx_draw_string(dpi, buffer, colour, x, y);
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

	buffer = (char*)0x0141ED68;
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
	int eax, ebx, ecx, edx, esi, edi, ebp;

	eax = colour;
	ebx = format;
	ecx = x;
	edx = y;
	esi = (int)args;
	edi = (int)dpi;
	ebp = width;
	RCT2_CALLFUNC_X(0x006C1E53, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	return (sint16)(edx & 0xFFFF) - y;
}

/**
 * 
 *  rct2: 0x006C2105
 * dpi (edi)
 * format (esi)
 * x (cx)
 * y (dx)
 * width (bp)
 * colour (bx)
 * unknown (al)
 */
int gfx_draw_string_left_wrapped(rct_drawpixelinfo *dpi, void *format, int x, int y, int width, int colour, int unknown)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	eax = unknown;
	ebx = colour;
	ecx = x;
	edx = y;
	esi = (int)format;
	edi = (int)dpi;
	ebp = width;
	RCT2_CALLFUNC_X(0x006C2105, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	return (sint16)(edx & 0xFFFF) - y;
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

	buffer = (char*)0x0141ED68;
	format_string(buffer, format, args);
	gfx_draw_string(dpi, buffer, colour, x, y);
}

/**
 * 
 *  rct2: 0x00682702
 * dpi (edi)
 * format (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 */
void gfx_draw_string(rct_drawpixelinfo *dpi, char *format, int colour, int x, int y)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	eax = colour;
	ebx = 0;
	ecx = x;
	edx = y;
	esi = (int)format;
	edi = (int)dpi;
	ebp = 0;
	RCT2_CALLFUNC_X(0x00682702, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	gLastDrawStringX = ecx;
	gLastDrawStringY = edx;
}
