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

#include "../addresses.h"
#include "../common.h"
#include "../localisation/localisation.h"
#include "../interface/window.h"
#include "../platform/platform.h"
#include "../object.h"
#include "../world/water.h"
#include "drawing.h"

// HACK These were originally passed back through registers
int gLastDrawStringX;
int gLastDrawStringY;

uint8 gGamePalette[256 * 4];
uint32 gPaletteEffectFrame;

uint32 gPickupPeepImage;
sint32 gPickupPeepX;
sint32 gPickupPeepY;

void *unk_9ABDA4;
void *unk_9E3CDC;
void *unk_9E3CE4[8];
#if NO_RCT2
rct_drawpixelinfo *unk_140E9A8;
#endif

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
const uint16 palette_to_g1_offset[PALETTE_TO_G1_OFFSET_COUNT] = {
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
	rct_g1_element g1 = g1Elements[pal];
	int width = g1.width;
	int x = g1.x_offset;
	uint8* dest_pointer = &gGamePalette[x * 4];
	uint8* source_pointer = g1.offset;

	for (; width > 0; width--) {
		dest_pointer[0] = (source_pointer[0] * product) >> 8;
		dest_pointer[1] = (source_pointer[1] * product) >> 8;
		dest_pointer[2] = (source_pointer[2] * product) >> 8;
		source_pointer += 3;
		dest_pointer += 4;
	}
	platform_update_palette(gGamePalette, 10, 236);
}

/**
 *
 *  rct2: 0x006837E3 
 */
void load_palette(){
	rct_water_type* water_type = (rct_water_type*)object_entry_groups[OBJECT_TYPE_WATER].chunks[0];

	uint32 palette = 0x5FC;

	if ((sint32)water_type != -1){
		palette = water_type->image_id;
	}

	rct_g1_element g1 = g1Elements[palette];
	int width = g1.width;
	int x = g1.x_offset;
	uint8* dest_pointer = &gGamePalette[x * 4];
	uint8* source_pointer = g1.offset;

	for (; width > 0; width--) {
		dest_pointer[0] = source_pointer[0];
		dest_pointer[1] = source_pointer[1];
		dest_pointer[2] = source_pointer[2];
		source_pointer += 3;
		dest_pointer += 4;
	}
	platform_update_palette(gGamePalette, 10, 236);
}

/**
 *
 *  rct2: 0x006ED7E5
 */
void gfx_invalidate_screen()
{
	gfx_set_dirty_blocks(0, 0, gScreenWidth, gScreenHeight);
}

/**
 *
 *  rct2: 0x006E7499
 * left (ax)
 * top (bx)
 * right (dx)
 * bottom (bp)
 */
void window_draw_all(rct_drawpixelinfo *dpi, short left, short top, short right, short bottom)
{
	rct_drawpixelinfo windowDPI;
	windowDPI.bits = dpi->bits + left + ((dpi->width + dpi->pitch) * top);
	windowDPI.x = left;
	windowDPI.y = top;
	windowDPI.width = right - left;
	windowDPI.height = bottom - top;
	windowDPI.pitch = dpi->width + dpi->pitch + left - right;
	windowDPI.zoom_level = 0;

	for (rct_window *w = g_window_list; w < gWindowNextSlot; w++) {
		if (w->flags & WF_TRANSPARENT) continue;
		if (right <= w->x || bottom <= w->y) continue;
		if (left >= w->x + w->width || top >= w->y + w->height) continue;

		window_draw(&windowDPI, w, left, top, right, bottom);
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
bool clip_drawpixelinfo(rct_drawpixelinfo *dst, rct_drawpixelinfo *src, int x, int y, int width, int height)
{
	int right = x + width;
	int bottom = y + height;

	dst->bits = src->bits;
	dst->x = src->x;
	dst->y = src->y;
	dst->width = src->width;
	dst->height = src->height;
	dst->pitch = src->pitch;
	dst->zoom_level = 0;

	if (x > dst->x) {
		uint16 clippedFromLeft = x - dst->x;
		dst->width -= clippedFromLeft;
		dst->x = x;
		dst->pitch += clippedFromLeft;
		dst->bits += clippedFromLeft;
	}

	int stickOutWidth = dst->x + dst->width - right;
	if (stickOutWidth > 0) {
		dst->width -= stickOutWidth;
		dst->pitch += stickOutWidth;
	}

	if (y > dst->y) {
		uint16 clippedFromTop = y - dst->y;
		dst->height -= clippedFromTop;
		dst->y = y;
		uint32 bitsPlus = (dst->pitch + dst->width) * clippedFromTop;
		dst->bits += bitsPlus;
	}

	int bp = dst->y + dst->height - bottom;
	if (bp > 0) {
		dst->height -= bp;
	}

	if (dst->width > 0 && dst->height > 0) {
		dst->x -= x;
		dst->y -= y;
		return true;
	}

	return false;
}

void gfx_invalidate_pickedup_peep()
{
	uint32 sprite = gPickupPeepImage;
	if (sprite != UINT32_MAX) {
		sprite = sprite & 0x7FFFF;

		rct_g1_element *g1_elements = &g1Elements[sprite];
		sint32 left = gPickupPeepX + g1_elements->x_offset;
		sint32 top = gPickupPeepY + g1_elements->y_offset;
		sint32 right = left + g1_elements->width;
		sint32 bottom = top + g1_elements->height;

		gfx_set_dirty_blocks(left, top, right, bottom);
	}
}

void gfx_draw_pickedup_peep(rct_drawpixelinfo *dpi)
{
	if (gPickupPeepImage != UINT32_MAX) {
		gfx_draw_sprite(dpi, gPickupPeepImage, gPickupPeepX, gPickupPeepY, 0);
	}
}
