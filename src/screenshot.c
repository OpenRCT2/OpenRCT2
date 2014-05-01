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

#include <windows.h>
#include "addresses.h"
#include "gfx.h"
#include "rct2.h"
#include "screenshot.h"
#include "strings.h"
#include "window_error.h"
#include <lodepng.h>
#include <stdio.h>

/**
 * 
 *  rct2: 0x006E3AEC
 */
void screenshot_check()
{
	int screenshotIndex;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREENSHOT_COUNTDOWN, uint8) != 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_SCREENSHOT_COUNTDOWN, uint8)--;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREENSHOT_COUNTDOWN, uint8) == 0) {
			RCT2_CALLPROC_EBPSAFE(0x00684218);
			screenshotIndex = screenshot_dump();
			RCT2_GLOBAL(0x013CE952, uint16) = STR_SCR_BMP;
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = screenshotIndex;
			RCT2_GLOBAL(0x009A8C29, uint8) |= 1;

			window_error_open(screenshotIndex == -1 ? STR_SCREENSHOT_FAILED : STR_SCREENSHOT_SAVED_AS, -1);
			RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;
			RCT2_CALLPROC_EBPSAFE(0x006843DC);
		}
	}
}

static int screenshot_get_next_path(char *path)
{
	int i;
	for (i = 1; i < 1000; i++) {
		RCT2_GLOBAL(0x013CE952, uint16) = i;

		// Glue together path and filename
		sprintf(path, "%sSCR%d.PNG", RCT2_ADDRESS(RCT2_ADDRESS_APP_PATH_SLASH, char), i);

		if (GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND)
			return i;
	}

	return -1;
}


/**
 * 
 *  rct2: 0x00683D20
 */
int screenshot_dump()
{
	int i, index, width, height, stride;
	char path[MAX_PATH] = "";

	unsigned char r, g, b, a = 255;

	unsigned char* png;
	size_t pngSize;
	LodePNGState state;

	// Get a free screenshot path
	if ((index = screenshot_get_next_path(path)) == -1)
		return -1;


	lodepng_state_init(&state);
	state.info_raw.colortype = LCT_PALETTE;

	// Get image size
	width = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	height = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
	stride = (width + 3) & 0xFFFFFFFC;

	for (i = 0; i < 246; i++) {
		b = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 0];
		g = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 1];
		r = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 2];

		lodepng_palette_add(&state.info_raw, r, g, b, a);
	}

	rct_drawpixelinfo *dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);

	unsigned int error = lodepng_encode(&png, &pngSize, dpi->bits, stride, dpi->height, &state);
	if (!error) lodepng_save_file(png, pngSize, path);

	if (error) fprintf(stderr, "error: %u: %s\n", error, lodepng_error_text(error));

	free(png);
	return index;
}
