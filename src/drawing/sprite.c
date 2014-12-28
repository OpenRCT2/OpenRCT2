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

typedef struct {
	uint32 num_entries;
	uint32 total_size;
} rct_g1_header;

void *_g1Buffer = NULL;

/**
 * 
 *  rct2: 0x00678998
 */
int gfx_load_g1()
{
	log_verbose("loading g1 graphics");

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
	log_fatal("Unable to load g1 graphics");
	return 0;
}

/**
 * Copies a sprite onto the buffer. There is no compression used on the sprite
 * image.
 *  rct2: 0x0067A690
 */
void gfx_bmp_sprite_to_buffer(uint8* palette_pointer, uint8* unknown_pointer, uint8* source_pointer, uint8* dest_pointer, rct_g1_element* source_image, rct_drawpixelinfo *dest_dpi, int height, int width, int image_type){
	uint8 zoom_level = dest_dpi->zoom_level;
	uint8 zoom_amount = 1 << zoom_level;
	//Requires use of palette?
	if (image_type & IMAGE_TYPE_USE_PALETTE){

		//Mix with another image?? and colour adjusted
		if (unknown_pointer!= NULL){ //Not tested. I can't actually work out when this code runs.
			unknown_pointer += source_pointer - source_image->offset;// RCT2_GLOBAL(0x9E3CE0, uint32);

			for (; height > 0; height -= zoom_amount){
				uint8* next_source_pointer = source_pointer + (uint32)(source_image->width * zoom_amount);
				uint8* next_unknown_pointer = unknown_pointer + (uint32)(source_image->width * zoom_amount);
				uint8* next_dest_pointer = dest_pointer + (dest_dpi->width / zoom_amount) + dest_dpi->pitch;

				for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, unknown_pointer += zoom_amount, dest_pointer++){
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
		for (; height > 0; height -= zoom_amount){
			uint8* next_source_pointer = source_pointer + (uint32)(source_image->width * zoom_amount);
			uint8* next_dest_pointer = dest_pointer + (dest_dpi->width / zoom_amount) + dest_dpi->pitch;
			for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++){
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
		for (; height > 0; height -= zoom_amount){
			uint8* next_source_pointer = source_pointer + (uint32)(source_image->width * zoom_amount);
			uint8* next_dest_pointer = dest_pointer + (dest_dpi->width / zoom_amount) + dest_dpi->pitch;

			for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++){
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
		for (; height > 0; height -= zoom_amount){
			uint8* next_source_pointer = source_pointer + (uint32)(source_image->width * zoom_amount);
			uint8* next_dest_pointer = dest_pointer + (dest_dpi->width / zoom_amount) + dest_dpi->pitch;

			for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++, source_pointer += zoom_amount){
				*dest_pointer = *source_pointer;
			}

			dest_pointer = next_dest_pointer;
			source_pointer = next_source_pointer;
		}
		return;
	}

	if (RCT2_GLOBAL(0x9E3CDC, uint32) != 0){//Not tested. I can't actually work out when this code runs.
		unknown_pointer += source_pointer - source_image->offset;

		for (; height > 0; height -= zoom_amount){
			uint8* next_source_pointer = source_pointer + (uint32)(source_image->width * zoom_amount);
			uint8* next_unknown_pointer = unknown_pointer + (uint32)(source_image->width * zoom_amount);
			uint8* next_dest_pointer = dest_pointer + (dest_dpi->width / zoom_amount) + dest_dpi->pitch;

			for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++, source_pointer += zoom_amount, unknown_pointer += zoom_amount){
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
	for (; height > 0; height -= zoom_amount){
		uint8* next_source_pointer = source_pointer + (uint32)(source_image->width * zoom_amount);
		uint8* next_dest_pointer = dest_pointer + (dest_dpi->width / zoom_amount) + dest_dpi->pitch;

		for (int no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++, source_pointer += zoom_amount){
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

/**
 * Transfers readied images onto buffers
 * This function copies the sprite data onto the screen
 *  rct2: 0x0067AA18
 */
void gfx_rle_sprite_to_buffer(uint8* source_bits_pointer, uint8* dest_bits_pointer, uint8* palette_pointer, rct_drawpixelinfo *dpi, int image_type, int source_y_start, int height, int source_x_start, int width){
	int zoom_level = dpi->zoom_level;
	int zoom_amount = 1 << zoom_level;
	int zoom_mask = 0xFFFFFFFF << zoom_level;
	uint8* next_source_pointer;
	uint8* next_dest_pointer = dest_bits_pointer;

	if (source_y_start < 0){ 
		source_y_start += zoom_amount; 
		next_dest_pointer += dpi->width / zoom_amount + dpi->pitch;
		height -= zoom_amount;
	}

	//For every line in the image
	for (int y = source_y_start; y < (height + source_y_start); y += zoom_amount){

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

			if (x_start & ~zoom_mask){
				no_pixels -= (x_start&~zoom_mask);
				x_start += (x_start&~zoom_mask);
				source_pointer += (x_start&~zoom_mask);
				if (no_pixels <= 0) continue;
			}

			if (x_start > 0){
				//Since the start is positive
				//We need to move the drawing surface to the correct position
				dest_pointer += x_start / zoom_amount;
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
				for (; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++){
					uint8 al = *source_pointer;
					uint8 ah = *dest_pointer;
					if (image_type & IMAGE_TYPE_MIX_BACKGROUND)
						al = palette_pointer[(((uint16)al << 8) | ah) - 0x100];
					else
						al = palette_pointer[al];
					*dest_pointer = al;
				}
			}
			else if (image_type & IMAGE_TYPE_MIX_BACKGROUND){//In the .exe these are all unraveled loops
				//Doesnt use source pointer ??? mix with background only?
				//Not Tested
				
				for (; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++){
					uint8 pixel = *dest_pointer;
					pixel = palette_pointer[pixel];
					*dest_pointer = pixel;
				}
			}
			else
			{
				for (; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++){
					*dest_pointer = *source_pointer;
				}
			}
		}

		//Add a line to the drawing surface pointer
		next_dest_pointer += dpi->width / zoom_amount + dpi->pitch;
	}
}

/**
 *
 *  rct2: 0x0067A28E
 * image_id (ebx) 
 * image_id as below
 * 0b_111X_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX image_type
 * 0b_XXX1_11XX_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX image_sub_type (unknown pointer)
 * 0b_XXX1_1111_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX secondary_colour
 * 0b_XXXX_XXXX_1111_1XXX_XXXX_XXXX_XXXX_XXXX primary_colour
 * 0b_XXXX_X111_1111_1XXX_XXXX_XXXX_XXXX_XXXX palette_ref
 * 0b_XXXX_XXXX_XXXX_X111_1111_1111_1111_1111 image_id (offset to g1)
 * x (cx)
 * y (dx)
 * dpi (esi)
 * tertiary_colour (ebp)
 */
void gfx_draw_sprite(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint32 tertiary_colour)
{
	//RCT2_CALLPROC_X(0x0067A28E, 0, image_id, x, y, 0, (int)dpi, tertiary_colour);
	//return;

	int image_type = (image_id & 0xE0000000) >> 28;
	int image_sub_type = (image_id & 0x1C000000) >> 26;

	uint8* palette_pointer = NULL;

	RCT2_GLOBAL(0x00EDF81C, uint32) = image_id & 0xE0000000;

	uint8* unknown_pointer = (uint8*)(RCT2_ADDRESS(0x9E3CE4, uint32*)[image_sub_type]);
	RCT2_GLOBAL(0x009E3CDC, uint32) = (uint32)unknown_pointer;

	if (image_type && !(image_type & IMAGE_TYPE_UNKNOWN)) {
		uint8 palette_ref = (image_id >> 19) & 0xFF;
		if (image_type & IMAGE_TYPE_MIX_BACKGROUND){
			unknown_pointer = NULL;
			RCT2_GLOBAL(0x009E3CDC, uint32) = 0;
		}
		else{
			palette_ref &= 0x7F;
		}

		uint16 palette_offset = palette_to_g1_offset[palette_ref];
		palette_pointer = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[palette_offset].offset;
	}
	else if (image_type && !(image_type & IMAGE_TYPE_USE_PALETTE)){
		RCT2_GLOBAL(0x9E3CDC, uint32) = 0;
		unknown_pointer = NULL;
		palette_pointer = RCT2_ADDRESS(0x9ABF0C, uint8);

		uint32 primary_offset = palette_to_g1_offset[(image_id >> 19) & 0x1F];
		uint32 secondary_offset = palette_to_g1_offset[(image_id >> 24) & 0x1F];
		uint32 tertiary_offset = palette_to_g1_offset[tertiary_colour];

		rct_g1_element* primary_colour = &RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[primary_offset];
		rct_g1_element* secondary_colour = &RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[secondary_offset];
		rct_g1_element* tertiary_colour = &RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[tertiary_offset];

		memcpy(palette_pointer + 0xF3, &primary_colour->offset[0xF3], 12);
		memcpy(palette_pointer + 0xCA, &secondary_colour->offset[0xF3], 12);
		memcpy(palette_pointer + 0x2E, &tertiary_colour->offset[0xF3], 12);

		//image_id
		RCT2_GLOBAL(0xEDF81C, uint32) |= 0x20000000;
		image_id |= IMAGE_TYPE_USE_PALETTE << 28;
	}
	else if (image_type){
		RCT2_GLOBAL(0x9E3CDC, uint32) = 0;
		unknown_pointer = NULL;

		palette_pointer = RCT2_ADDRESS(0x9ABE0C, uint8);

		//Top
		int top_type = (image_id >> 19) & 0x1f;
		uint32 top_offset = palette_to_g1_offset[top_type]; //RCT2_ADDRESS(0x97FCBC, uint32)[top_type];
		rct_g1_element top_palette = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[top_offset];
		memcpy(palette_pointer + 0xF3, top_palette.offset + 0xF3, 12);
		
		//Trousers
		int trouser_type = (image_id >> 24) & 0x1f;
		uint32 trouser_offset = palette_to_g1_offset[trouser_type]; //RCT2_ADDRESS(0x97FCBC, uint32)[trouser_type];
		rct_g1_element trouser_palette = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[trouser_offset];
		memcpy(palette_pointer + 0xCA, trouser_palette.offset + 0xF3, 12);
	}

	//For backwards compatibility
	RCT2_GLOBAL(0x9ABDA4, uint8*) = palette_pointer;

	gfx_draw_sprite_palette_set(dpi, image_id, x, y, palette_pointer, unknown_pointer);
}

/*
* rct: 0x0067A46E
* image_id (ebx) and also (0x00EDF81C)
* palette_pointer (0x9ABDA4)
* unknown_pointer (0x9E3CDC)
* dpi (edi)
* x (cx)
* y (dx)
*/
void gfx_draw_sprite_palette_set(rct_drawpixelinfo *dpi, int image_id, int x, int y, uint8* palette_pointer, uint8* unknown_pointer){
	int image_element = 0x7FFFF&image_id;
	int image_type = (image_id & 0xE0000000) >> 28;
	
	rct_g1_element* g1_source = &(RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[image_element]);

	//Zooming code has been integrated into main code.
	//if (dpi->zoom_level >= 1){ //These have not been tested
	//	//something to do with zooming
	//	if (dpi->zoom_level == 1){
	//		RCT2_CALLPROC_X(0x0067A28E, 0, image_id, x, y, 0, (int)dpi, 0);
	//		return;
	//	}
	//	if (dpi->zoom_level == 2){
	//		RCT2_CALLPROC_X(0x0067DADA, 0, (int)g1_source, x, y, 0, (int)dpi, 0);
	//		return;
	//	}
	//	RCT2_CALLPROC_X(0x0067FAAE, 0, (int)g1_source, x, y, 0, (int)dpi, 0);
	//	return;
	//}
	if ( dpi->zoom_level && (g1_source->flags & (1<<4)) ){
		rct_drawpixelinfo zoomed_dpi = {
			.bits = dpi->bits,
			.x = dpi->x >> 1,
			.y = dpi->y >> 1,
			.height = dpi->height>>1,
			.width = dpi->width>>1,
			.pitch = dpi->pitch,
			.zoom_level = dpi->zoom_level - 1
		};
		gfx_draw_sprite_palette_set(&zoomed_dpi, (image_type << 28) | (image_element - g1_source->zoomed_offset), x >> 1, y >> 1, palette_pointer, unknown_pointer);
		return;
	}

	if ( dpi->zoom_level && (g1_source->flags & (1<<5)) ){
		return;
	}

	//Its used super often so we will define it to a seperate variable.
	int zoom_level = dpi->zoom_level;
	int zoom_amount = 1 << zoom_level;
	int zoom_mask = 0xFFFFFFFF << zoom_level;

	if (zoom_level && g1_source->flags & G1_FLAG_RLE_COMPRESSION){
		x -= ~zoom_mask;
		y -= ~zoom_mask;
	}

	//This will be the height of the drawn image
	int height = g1_source->height;
	//This is the start y coordinate on the destination
	sint16 dest_start_y = y + g1_source->y_offset;

	// For whatever reason the RLE version does not use
	// the zoom mask on the y coordinate but does on x.
	if (g1_source->flags & G1_FLAG_RLE_COMPRESSION){
		dest_start_y -= dpi->y;
	}
	else{
		dest_start_y = (dest_start_y&zoom_mask) - dpi->y;
	}
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
		source_start_y -= dest_start_y;
		//The destination start is now reset to 0
		dest_start_y = 0;
	}
	else{
		if (g1_source->flags & G1_FLAG_RLE_COMPRESSION && zoom_level){
			source_start_y -= dest_start_y & ~zoom_mask;
			height += dest_start_y & ~zoom_mask;
		}
	}

	int dest_end_y = dest_start_y + height;

	if (dest_end_y > dpi->height){
		//If the destination y is outside of the drawing
		//image reduce the height of the image
		height -= dest_end_y - dpi->height;
	}
	//If the image no longer has anything to draw
	if (height <= 0)return;

	dest_start_y /= zoom_amount;
	dest_end_y /= zoom_amount;

	//This will be the width of the drawn image
	int width = g1_source->width;
	//This is the source start x coordinate
	int source_start_x = 0;
	//This is the destination start x coordinate
	sint16 dest_start_x = ((x + g1_source->x_offset + ~zoom_mask)&zoom_mask) - dpi->x;

	if (dest_start_x < 0){
		//If the destination is negative reduce the width
		//image will cut off the side
		width += dest_start_x;
		//If there is no image to draw
		if (width <= 0){
			return;
		}
		//The source start will also need to cut off the side
		source_start_x -= dest_start_x;
		//Reset the destination to 0
		dest_start_x = 0;
	}
	else{
		if (g1_source->flags & G1_FLAG_RLE_COMPRESSION && zoom_level){
			source_start_x -= dest_start_x & ~zoom_mask;
		}
	}

	int dest_end_x = dest_start_x + width;

	if (dest_end_x > dpi->width){
		//If the destination x is outside of the drawing area
		//reduce the image width.
		width -= dest_end_x - dpi->width;
		//If there is no image to draw.
		if (width <= 0)return;
	}

	dest_start_x /= zoom_amount;
	dest_end_x /= zoom_amount;

	uint8* dest_pointer = (uint8*)dpi->bits;
	//Move the pointer to the start point of the destination
	dest_pointer += ((dpi->width / zoom_amount) + dpi->pitch)*dest_start_y + dest_start_x;
	
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