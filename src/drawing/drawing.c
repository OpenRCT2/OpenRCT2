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

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include <limits.h>
#include "../addresses.h"
#include "../common.h"
#include "../localisation/localisation.h"
#include "../interface/window.h"
#include "../platform/osinterface.h"
#include "drawing.h"

// HACK These were originally passed back through registers
int gLastDrawStringX;
int gLastDrawStringY;

uint8 _screenDirtyBlocks[5120];

//Originally 0x9ABE0C, 12 elements from 0xF3 are the peep top colour, 12 elements from 0xCA are peep trouser colour
const uint8 peep_palette[0x100] = { 
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

// Previously 0x97FCBC use it to get the correct palette from g1_elements
const uint16 palette_to_g1_offset[] = {
	0x1333, 0x1334, 0x1335, 0x1336,
	0x1337, 0x1338, 0x1339, 0x133A,
	0x133B, 0x133C, 0x133D, 0x133E,
	0x133F, 0x1340, 0x1341, 0x1342,
	0x1343, 0x1344, 0x1345, 0x1346,
	0x1347, 0x1348, 0x1349, 0x134A,
	0x134B, 0x134C, 0x134D, 0x134E,
	0x134F, 0x1350, 0x1351, 0x1352,
	0x1353, 0x0C1C, 0x0C1D, 0x0C1E,
	0x0C1F, 0x0C20, 0x0C22, 0x0C23,
	0x0C24, 0x0C25, 0x0C26, 0x0C21,
	0x1354, 0x1355, 0x1356, 0x1357,
	0x1358, 0x1359, 0x135A, 0x135B,
	0x135C, 0x135D, 0x135E, 0x135F,
	0x1360, 0x1361, 0x1362, 0x1363,
	0x1364, 0x1365, 0x1366, 0x1367,
	0x1368, 0x1369, 0x136A, 0x136B,
	0x136C, 0x136D, 0x136E, 0x136F,
	0x1370, 0x1371, 0x1372, 0x1373,
	0x1374, 0x1375, 0x1376, 0x1377,
	0x1378, 0x1379, 0x137A, 0x137B,
	0x137C, 0x137D, 0x137E, 0x137F,
	0x1380, 0x1381, 0x1382, 0x1383,
	0x1384, 0x1385, 0x1386, 0x1387,
	0x1388, 0x1389, 0x138A, 0x138B,
	0x138C, 0x138D, 0x138E, 0x138F,
	0x1390, 0x1391, 0x1392, 0x1393,
	0x1394, 0x1395, 0x1396, 0x1397,
	0x1398, 0x1399, 0x139A, 0x139B,
	0x139C, 0x139D, 0x139E, 0x139F,
	0x13A0, 0x13A1, 0x13A2, 0x13A3,
	0x13A4, 0x13A5, 0x13A6, 0x13A7,
	0x13A8, 0x13A9, 0x13AA, 0x13AB,
	0x13AC, 0x13AD, 0x13AE, 0x13AF,
	0x13B0, 0x13B1, 0x13B2, 0x13B3,
	0x13B4, 0x13B5, 0x13B6, 0x13B7,
};

static void gfx_draw_dirty_blocks(int x, int y, int columns, int rows);

/**
 * Clears the screen with the specified colour.
 *  rct2: 0x00678A9F
 */
void gfx_clear(rct_drawpixelinfo *dpi, int colour)
{
	int y, w, h;
	char* ptr;

	w = dpi->width >> dpi->zoom_level;
	h = dpi->height >> dpi->zoom_level;

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

/**
 *
 *  rct2: 0x00683854
 * a1 (ebx)
 * product (cl)
 */
void gfx_transpose_palette(int pal, unsigned char product)
{
	rct_g1_element g1 = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[pal];
	int width = g1.width;
	int x = g1.x_offset;  
	uint8* dest_pointer = (uint8*)&(RCT2_ADDRESS(0x014124680,uint8)[x]);
	uint8* source_pointer = g1.offset;

	for (; width > 0; width--) {
		dest_pointer[0] = (source_pointer[0] * product) >> 8;
		dest_pointer[1] = (source_pointer[1] * product) >> 8;
		dest_pointer[2] = (source_pointer[2] * product) >> 8;
		source_pointer += 3;
		dest_pointer += 4;
	}
	osinterface_update_palette((char*)0x01424680, 10, 236);//Odd would have expected dest_pointer
}

/**
*
*  rct2: 0x006EC9CE
* @param x (ax)
* @param y (cx)
* @param base_height (di)
* @param clearance_height (si)
*/
void gfx_invalidate_scrollingtext(int x, int y, int base_height, int clearance_height)
{
	x += 16;
	y += 16;
	int left, top, right, bottom;
	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)) {
		case 0:
			left = (-x + y) - 32;
			right = (-x + y) + 32;
			top = ((y + x) / 2) - 32 - clearance_height;
			bottom = ((y + x) / 2) + 32 - base_height;
			break;
		case 1:
			left = (-x - y) - 32;
			right = (-x - y) + 32;
			top = ((y - x) / 2) - 32 - clearance_height;
			bottom = ((y - x) / 2) + 32 - base_height;
			break;
		case 2:
			left = (x - y) - 32;
			right = (x - y) + 32;
			top = ((-y - x) / 2) - 32 - clearance_height;
			bottom = ((-y - x) / 2) + 32 - base_height;
			break;
		case 3:
			left = (x + y) - 32;
			right = (x + y) + 32;
			top = ((-y + x) / 2) - 32 - clearance_height;
			bottom = ((-y + x) / 2) + 32 - base_height;
			break;
	}
	for (rct_viewport** viewport_p = RCT2_ADDRESS(RCT2_ADDRESS_ACTIVE_VIEWPORT_PTR_ARRAY, rct_viewport*); *viewport_p; ++viewport_p)
	{
		rct_viewport* viewport = *viewport_p;
		if (viewport->zoom < 1) {
			if (right > viewport->view_x && bottom > viewport->view_y && left < viewport->view_x + viewport->view_width) {
				if (left < viewport->view_x) {
					left = viewport->view_x;
				}
				if (right > viewport->view_x + viewport->view_width) {
					right = viewport->view_x + viewport->view_width;
				}
				if (top < viewport->view_y + viewport->view_height) {
					if (top < viewport->view_y) {
						top = viewport->view_y;
					}
					if (bottom > viewport->view_y + viewport->view_height) {
						bottom = viewport->view_y + viewport->view_height;
					}
					left = ((left - viewport->view_x) >> viewport->zoom) + viewport->x;
					top = ((top - viewport->view_y) >> viewport->zoom) + viewport->y;
					right = ((right - viewport->view_x) >> viewport->zoom) + viewport->x;
					bottom = ((bottom - viewport->view_y) >> viewport->zoom) + viewport->y;
					gfx_set_dirty_blocks(left, top, right, bottom);
				}
			}
		}
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

	for (w = g_window_list; w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++) {
		if (w->flags & WF_TRANSPARENT)
			continue;
		if (right <= w->x || bottom <= w->y)
			continue;
		if (left >= w->x + w->width || top >= w->y + w->height)
			continue;
		window_draw(w, left, top, right, bottom);
	}
}

/*
*
* rct2: 0x006EE53B
* left (ax)
* width (bx)
* top (cx)
* height (dx)
* drawpixelinfo (edi)
*/
rct_drawpixelinfo* clip_drawpixelinfo(rct_drawpixelinfo* dpi, int left, int width, int top, int height)
{
	rct_drawpixelinfo* newDrawPixelInfo = rct2_malloc(sizeof(rct_drawpixelinfo));

	int right = left + width;
	int bottom = top + height;

	newDrawPixelInfo->bits = dpi->bits;
	newDrawPixelInfo->x = dpi->x;
	newDrawPixelInfo->y = dpi->y;
	newDrawPixelInfo->width = dpi->width;
	newDrawPixelInfo->height = dpi->height;
	newDrawPixelInfo->pitch = dpi->pitch;
	newDrawPixelInfo->zoom_level = 0;

	if (left > newDrawPixelInfo->x) {
		uint16 clippedFromLeft = left - newDrawPixelInfo->x;
		newDrawPixelInfo->width -= clippedFromLeft;
		newDrawPixelInfo->x = left;
		newDrawPixelInfo->pitch += clippedFromLeft;
		newDrawPixelInfo->bits += clippedFromLeft;
	}

	int stickOutWidth = newDrawPixelInfo->x + newDrawPixelInfo->width - right;
	if (stickOutWidth > 0) {
		newDrawPixelInfo->width -= stickOutWidth;
		newDrawPixelInfo->pitch += stickOutWidth;
	}

	if (top > newDrawPixelInfo->y) {
		uint16 clippedFromTop = top - newDrawPixelInfo->y;
		newDrawPixelInfo->height -= clippedFromTop;
		newDrawPixelInfo->y = top;
		uint32 bitsPlus = (newDrawPixelInfo->pitch + newDrawPixelInfo->width) * clippedFromTop;
		newDrawPixelInfo->bits += bitsPlus;
	}

	int bp = newDrawPixelInfo->y + newDrawPixelInfo->height - bottom;
	if (bp > 0) {
		newDrawPixelInfo->height -= bp;
	}

	if (newDrawPixelInfo->width > 0 && newDrawPixelInfo->height > 0) {
		newDrawPixelInfo->x -= left;
		newDrawPixelInfo->y -= top;

		return newDrawPixelInfo;
	}

	rct2_free(newDrawPixelInfo);
	return NULL;
}

/***
*
* rct2: 0x00684027
*
* ebp used to be a parameter but it is always zero
* left   : eax
* top    : ebx
* width  : ecx
* height : edx
* x_start: edi
* y_start: esi
*/
void gfx_draw_rain(int left, int top, int width, int height, sint32 x_start, sint32 y_start){
	uint8* pattern = RCT2_GLOBAL(RCT2_ADDRESS_RAIN_PATTERN, uint8*);
	uint8 pattern_x_space = *pattern++;
	uint8 pattern_y_space = *pattern++;

	uint8 pattern_start_x_offset = x_start % pattern_x_space;
	uint8 pattern_start_y_offset = y_start % pattern_y_space;;

	rct_drawpixelinfo* dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	uint32 pixel_offset = (dpi->pitch + dpi->width)*top + left;
	uint8 pattern_y_pos = pattern_start_y_offset;

	//Stores the colours of changed pixels
	uint32* pixel_store = RCT2_ADDRESS(RCT2_ADDRESS_RAIN_PIXEL_STORE, uint32);
	pixel_store += RCT2_GLOBAL(RCT2_ADDRESS_NO_RAIN_PIXELS, uint32);

	for (; height != 0; height--){
		uint8 pattern_x = pattern[pattern_y_pos * 2];
		if (pattern_x != 0xFF){
			if (RCT2_GLOBAL(0x9AC00C, uint32) <= 0x1F38){

				int final_pixel_offset = width + pixel_offset;

				int x_pixel_offset = pixel_offset;
				x_pixel_offset += ((uint8)(pattern_x - pattern_start_x_offset)) % pattern_x_space;

				uint8 pattern_pixel = pattern[pattern_y_pos * 2 + 1];
				for (; x_pixel_offset < final_pixel_offset; x_pixel_offset += pattern_x_space){
					uint8 current_pixel = dpi->bits[x_pixel_offset];
					dpi->bits[x_pixel_offset] = pattern_pixel;
					RCT2_GLOBAL(RCT2_ADDRESS_NO_RAIN_PIXELS, uint32)++;

					//Store colour and position
					*pixel_store++ = (x_pixel_offset << 8) | current_pixel;
				}
			}
		}

		pixel_offset += dpi->pitch + dpi->width;

		pattern_y_pos++;
		pattern_y_pos %= pattern_y_space;
	}
}

/**
*
*  rct2: 0x006843DC
*/
void redraw_peep_and_rain()
{
	if (RCT2_GLOBAL(0x009ABDF2, uint32) != 0) {
		int sprite = RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, sint32);
		if (sprite != -1) {
			sprite = sprite & 0x7FFFF;
			
			rct_g1_element *g1_elements = &RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[sprite];
			int left = RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_X, sint16) + g1_elements->x_offset;
			int top = RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_Y, sint16) + g1_elements->y_offset;
			int right = left + g1_elements->width;
			int bottom = top + g1_elements->height;
			
			gfx_set_dirty_blocks(left, top, right, bottom);
		}

		int rain_no_pixels = RCT2_GLOBAL(RCT2_ADDRESS_NO_RAIN_PIXELS, uint32);
		if (rain_no_pixels == 0) {
			return;
		}
		rct_window *window = window_get_main();
		uint32 numPixels = window->width * window->height;
		
		uint32 *rain_pixels = RCT2_ADDRESS(RCT2_ADDRESS_RAIN_PIXEL_STORE, uint32);
		if (rain_pixels) {
			uint8 *screen_pixels = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo)->bits;
			for (int i = 0; i < rain_no_pixels; i++) {
				uint32 pixel = rain_pixels[i];
				//HACK
				if (pixel >> 8 > numPixels) {
					log_verbose("Pixel error, skipping rain draw in this frame");
					break;
				}
				screen_pixels[pixel >> 8] = pixel & 0xFF;
			}
			RCT2_GLOBAL(0x009E2C78, uint32) = 1;
		}
	}
	RCT2_GLOBAL(RCT2_ADDRESS_NO_RAIN_PIXELS, uint32) = 0;
}
