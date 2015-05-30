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
#pragma pack(1)

#include <lodepng/lodepng.h>
#include "../addresses.h"
#include "../config.h"
#include "../drawing/drawing.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../openrct2.h"
#include "../platform/platform.h"
#include "../windows/error.h"
#include "screenshot.h"
#include "viewport.h"

static const char *_screenshot_format_extension[] = { ".bmp", ".png" };

static int screenshot_dump_bmp();
static int screenshot_dump_png();

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
			update_rain_animation();
			screenshotIndex = screenshot_dump();

			if (screenshotIndex != -1) {
				char *lang_3165 = (char*)0x009BC677;
				sprintf(lang_3165, "SCR%d%s", screenshotIndex, _screenshot_format_extension[gConfigGeneral.screenshot_format]);

				RCT2_GLOBAL(0x013CE952, uint16) = 3165;
				// RCT2_GLOBAL(0x013CE952, uint16) = STR_SCR_BMP;
				// RCT2_GLOBAL(0x013CE952 + 2, uint16) = screenshotIndex;
				RCT2_GLOBAL(0x009A8C29, uint8) |= 1;

				window_error_open(STR_SCREENSHOT_SAVED_AS, -1);
			} else {
				window_error_open(STR_SCREENSHOT_FAILED, -1);
			}

			RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;
			redraw_peep_and_rain();
		}
	}
}

static int screenshot_get_next_path(char *path, int format)
{
	char screenshotPath[MAX_PATH];

	platform_get_user_directory(screenshotPath, "screenshot");
	if (!platform_ensure_directory_exists(screenshotPath)) {
		fprintf(stderr, "Unable to save screenshots in OpenRCT2 screenshot directory.\n");
		return -1;
	}

	int i;
	for (i = 1; i < 1000; i++) {
		RCT2_GLOBAL(0x013CE952, uint16) = i;

		// Glue together path and filename
		sprintf(path, "%sSCR%d%s", screenshotPath, i, _screenshot_format_extension[format]);

		if (!platform_file_exists(path)) {
			return i;
		}
	}

	fprintf(stderr, "You have too many saved screenshots.\n");
	return -1;
}

int screenshot_dump()
{
	switch (gConfigGeneral.screenshot_format) {
	case SCREENSHOT_FORMAT_BMP:
		return screenshot_dump_bmp();
	case SCREENSHOT_FORMAT_PNG:
		return screenshot_dump_png();
	default:
		return -1;
	}
}

// Bitmap header structs, for cross platform purposes
typedef struct {
	uint16 bfType;
	uint32 bfSize;
	uint16 bfReserved1;
	uint16 bfReserved2;
	uint32 bfOffBits;
} BitmapFileHeader;

typedef struct {
	uint32 biSize;
	sint32 biWidth;
	sint32 biHeight;
	uint16 biPlanes;
	uint16 biBitCount;
	uint32 biCompression;
	uint32 biSizeImage;
	sint32 biXPelsPerMeter;
	sint32 biYPelsPerMeter;
	uint32 biClrUsed;
	uint32 biClrImportant;
} BitmapInfoHeader;

/**
 * 
 *  rct2: 0x00683D20
 */
int screenshot_dump_bmp()
{
	BitmapFileHeader header;
	BitmapInfoHeader info;

	int i, y, index, width, height, stride;
	char *buffer, path[MAX_PATH], *row;
	FILE *fp;
	unsigned int bytesWritten;

	// Get a free screenshot path
	if ((index = screenshot_get_next_path(path, SCREENSHOT_FORMAT_BMP)) == -1)
		return -1;

	// Open binary file for writing
	if ((fp = fopen(path, "wb")) == NULL){
		return -1;
	}

	// Allocate buffer
	buffer = malloc(0xFFFF);
	if (buffer == NULL) {
		//CloseHandle(hFile);
		fclose(fp);
		return -1;
	}

	// Get image size
	width = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	height = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
	stride = (width + 3) & 0xFFFFFFFC;

	// File header
	memset(&header, 0, sizeof(header));
	header.bfType = 0x4D42;
	header.bfSize = height * stride + 1038;
	header.bfOffBits = 1038;

	bytesWritten = fwrite(&header, sizeof(BitmapFileHeader), 1, fp);
	if (bytesWritten != 1) {
		fclose(fp);
		free(buffer);
	}

	// Info header
	memset(&info, 0, sizeof(info));
	info.biSize = sizeof(info);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;
	info.biBitCount = 8;
	info.biXPelsPerMeter = 2520;
	info.biYPelsPerMeter = 2520;
	info.biClrUsed = 246;

	bytesWritten=fwrite(&info, sizeof(BitmapInfoHeader), 1, fp); 
	if (bytesWritten != 1) {
		fclose(fp);
		free(buffer);
	}

	// Palette
	memset(buffer, 0, 246 * 4);
	for (i = 0; i < 246; i++) {
		buffer[i * 4 + 0] = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 0];
		buffer[i * 4 + 1] = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 1];
		buffer[i * 4 + 2] = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 2];
	}

	bytesWritten = fwrite(buffer, sizeof(char), 246*4,  fp);
	if (bytesWritten != 246*4){
		fclose(fp);
		free(buffer);
	}

	// Image, save upside down
	rct_drawpixelinfo *dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	for (y = dpi->height - 1; y >= 0; y--) {
		row = dpi->bits + y * (dpi->width + dpi->pitch);

		memset(buffer, 0, stride);
		memcpy(buffer, row, dpi->width);

		bytesWritten=fwrite(buffer, sizeof(char), stride, fp);
		if (bytesWritten != stride){
			fclose(fp);
			free(buffer);
		}
	}

	fclose(fp);
	free(buffer);

	return index;
}

int screenshot_dump_png()
{
	rct_drawpixelinfo *dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);

	int i, index, width, height, stride;
	char path[MAX_PATH] = "";
	unsigned int error;
	unsigned char r, g, b, a = 255;

	unsigned char* png;
	size_t pngSize;
	LodePNGState state;

	// Get a free screenshot path
	if ((index = screenshot_get_next_path(path, SCREENSHOT_FORMAT_PNG)) == -1)
		return -1;


	lodepng_state_init(&state);
	state.info_raw.colortype = LCT_PALETTE;

	// Get image size
	width = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	height = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
	stride = (width + 3) & ~3;

	for (i = 0; i < 256; i++) {
		b = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 0];
		g = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 1];
		r = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 2];

		lodepng_palette_add(&state.info_raw, r, g, b, a);
	}

	error = lodepng_encode(&png, &pngSize, dpi->bits, stride, dpi->height, &state);
	if (error) {
		log_error("Unable to save screenshot, %u: %s", lodepng_error_text(error));
		index = -1;
	} else {
		lodepng_save_file(png, pngSize, path);
	}

	free(png);
	return index;
}

bool screenshot_write_png(rct_drawpixelinfo *dpi, const char *path)
{
	unsigned int error;
	unsigned char* png;
	size_t pngSize;
	LodePNGState state;

	lodepng_state_init(&state);
	state.info_raw.colortype = LCT_PALETTE;

	// Get image size
	int stride = (dpi->width + 3) & ~3;

	for (int i = 0; i < 256; i++) {
		unsigned char r, g, b, a = 255;

		b = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 0];
		g = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 1];
		r = RCT2_ADDRESS(0x01424680, uint8)[i * 4 + 2];

		lodepng_palette_add(&state.info_raw, r, g, b, a);
	}

	error = lodepng_encode(&png, &pngSize, dpi->bits, stride, dpi->height, &state);
	if (error != 0) {
		free(png);
		return false;
	} else {
		error = lodepng_save_file(png, pngSize, path);
		if (error != 0) {
			free(png);
			return false;
		}
	}

	free(png);
	return true;
}

int cmdline_for_screenshot(const char **argv, int argc)
{
	if (argc != 4 && argc != 8) {
		printf("Usage: openrct2 screenshot <file> <ouput_image> <width> <height> [<x> <y> <zoom> <rotation>]\n");
		return -1;
	}

	bool customLocation = false;
	bool centreMapX = false;
	bool centreMapY = false;
	int customX, customY, customZoom, customRotation;
	
	const char *inputPath = argv[0];
	const char *outputPath = argv[1];
	int resolutionWidth = atoi(argv[2]);
	int resolutionHeight = atoi(argv[3]);
	if (argc == 8) {
		customLocation = true;
		if (argv[4][0] == 'c')
			centreMapX = true;
		else
			customX = atoi(argv[4]);
		if (argv[5][0] == 'c')
			centreMapY = true;
		else
			customY = atoi(argv[5]);

		customZoom = atoi(argv[6]);
		customRotation = atoi(argv[7]) & 3;
	}

	gOpenRCT2Headless = true;
	if (openrct2_initialise()) {
		rct2_open_file(inputPath);

		RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;

		rct_viewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = resolutionWidth;
		viewport.height = resolutionHeight;
		viewport.view_width = viewport.width;
		viewport.view_height = viewport.height;
		viewport.var_11 = 0;
		viewport.flags = 0;

		if (customLocation) {
			if (centreMapX)
				customX = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint8) / 2) * 32 + 16;
			if (centreMapY)
				customY = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint8) / 2) * 32 + 16;

			int x, y;
			int z = map_element_height(customX, customY) & 0xFFFF;
			switch (customRotation) {
			case 0:
				x = customY - customX;
				y = ((customX + customY) / 2) - z;
				break;
			case 1:
				x = -customY - customX;
				y = ((-customX + customY) / 2) - z;
				break;
			case 2:
				x = -customY + customX;
				y = ((-customX - customY) / 2) - z;
				break;
			case 3:
				x = customY + customX;
				y = ((customX - customY) / 2) - z;
				break;
			}

			viewport.view_x = x - (viewport.view_width / 2);
			viewport.view_y = y - (viewport.view_height / 2);
			viewport.zoom = customZoom;

			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) = customRotation;
		} else {
			viewport.view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16) - (viewport.view_width / 2);
			viewport.view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16) - (viewport.view_height / 2);
			viewport.zoom = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) & 0xFF;

			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) >> 8;
		}

		rct_drawpixelinfo dpi;
		dpi.x = 0;
		dpi.y = 0;
		dpi.width = resolutionWidth;
		dpi.height = resolutionHeight;
		dpi.pitch = 0;
		dpi.zoom_level = 0;
		dpi.bits = malloc(dpi.width * dpi.height);

		viewport_render(&dpi, &viewport, 0, 0, viewport.width, viewport.height);

		screenshot_write_png(&dpi, outputPath);

		free(dpi.bits);
	}
	openrct2_dispose();
	return 1;
}