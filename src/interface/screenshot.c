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

#ifdef USE_LIBPNG
	#include <png.h>
#else
	#include <lodepng/lodepng.h>
#endif

#include "../addresses.h"
#include "../config.h"
#include "../drawing/drawing.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../openrct2.h"
#include "../platform/platform.h"
#include "../util/util.h"
#include "../windows/error.h"
#include "screenshot.h"
#include "viewport.h"

static const char *_screenshot_format_extension[] = { ".bmp", ".png" };

static int screenshot_dump_bmp();
static int screenshot_dump_png();

bool screenshot_write_png(rct_drawpixelinfo *dpi, const char *path);

#ifdef USE_LIBPNG
	static void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
	static void my_png_flush(png_structp png_ptr);
#endif

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
				rct_string_id stringId = 3165;
				sprintf((char*)language_get_string(stringId), "SCR%d%s", screenshotIndex, _screenshot_format_extension[gConfigGeneral.screenshot_format]);

				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = stringId;
				// RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = STR_SCR_BMP;
				// RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint16) = screenshotIndex;
				RCT2_GLOBAL(0x009A8C29, uint8) |= 1;

				window_error_open(STR_SCREENSHOT_SAVED_AS, -1);
			} else {
				window_error_open(STR_SCREENSHOT_FAILED, -1);
			}

			RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;
			redraw_rain();
		}
	}
}

static int screenshot_get_next_path(char *path, int format)
{
	char screenshotPath[MAX_PATH];

	platform_get_user_directory(screenshotPath, "screenshot");
	if (!platform_ensure_directory_exists(screenshotPath)) {
		log_error("Unable to save screenshots in OpenRCT2 screenshot directory.\n");
		return -1;
	}

	int i;
	for (i = 1; i < 1000; i++) {
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = i;

		// Glue together path and filename
		sprintf(path, "%sSCR%d%s", screenshotPath, i, _screenshot_format_extension[format]);

		if (!platform_file_exists(path)) {
			return i;
		}
	}

	log_error("You have too many saved screenshots.\n");
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
	char path[MAX_PATH];
	uint8 *buffer, *row;
	SDL_RWops *fp;
	unsigned int bytesWritten;

	// Get a free screenshot path
	if ((index = screenshot_get_next_path(path, SCREENSHOT_FORMAT_BMP)) == -1)
		return -1;

	// Open binary file for writing
	if ((fp = SDL_RWFromFile(path, "wb")) == NULL){
		return -1;
	}

	// Allocate buffer
	buffer = malloc(0xFFFF);
	if (buffer == NULL) {
		SDL_RWclose(fp);
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

	bytesWritten = SDL_RWwrite(fp, &header, sizeof(BitmapFileHeader), 1);
	if (bytesWritten != 1) {
		SDL_RWclose(fp);
		SafeFree(buffer);
		log_error("failed to save screenshot");
		return -1;
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

	bytesWritten = SDL_RWwrite(fp, &info, sizeof(BitmapInfoHeader), 1);
	if (bytesWritten != 1) {
		SDL_RWclose(fp);
		SafeFree(buffer);
		log_error("failed to save screenshot");
		return -1;
	}

	// Palette
	memset(buffer, 0, 246 * 4);
	for (i = 0; i < 246; i++) {
		buffer[i * 4 + 0] = RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8)[i * 4 + 0];
		buffer[i * 4 + 1] = RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8)[i * 4 + 1];
		buffer[i * 4 + 2] = RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8)[i * 4 + 2];
	}

	bytesWritten = SDL_RWwrite(fp, buffer, sizeof(char), 246 * 4);
	if (bytesWritten != 246*4){
		SDL_RWclose(fp);
		SafeFree(buffer);
		log_error("failed to save screenshot");
		return -1;
	}

	// Image, save upside down
	rct_drawpixelinfo *dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	for (y = dpi->height - 1; y >= 0; y--) {
		row = dpi->bits + y * (dpi->width + dpi->pitch);

		memset(buffer, 0, stride);
		memcpy(buffer, row, dpi->width);

		bytesWritten = SDL_RWwrite(fp, buffer, sizeof(char), stride);
		if (bytesWritten != stride){
			SDL_RWclose(fp);
			SafeFree(buffer);
			log_error("failed to save screenshot");
			return -1;
		}
	}

	SDL_RWclose(fp);
	free(buffer);

	return index;
}

int screenshot_dump_png()
{
	// Get a free screenshot path
	int index;
	char path[MAX_PATH] = "";
	if ((index = screenshot_get_next_path(path, SCREENSHOT_FORMAT_PNG)) == -1) {
		return -1;
	}

	rct_drawpixelinfo *dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	if (screenshot_write_png(dpi, path)) {
		return index;
	} else {
		return -1;
	}
}

bool screenshot_write_png(rct_drawpixelinfo *dpi, const char *path)
{
#ifdef USE_LIBPNG
	// Get image size
	int stride = dpi->width + dpi->pitch;

	// Setup PNG
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		return false;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return false;
	}

	png_colorp palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));
	for (int i = 0; i < 256; i++) {
		palette[i].blue		= RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8)[i * 4 + 0];
		palette[i].green	= RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8)[i * 4 + 1];
		palette[i].red		= RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8)[i * 4 + 2];
	}

	png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);

	// Open file for writing
	SDL_RWops *file = SDL_RWFromFile(path, "wb");
	if (file == NULL) {
		png_free(png_ptr, palette);
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return false;
	}
	png_set_write_fn(png_ptr, file, my_png_write_data, my_png_flush);

	// Set error handler
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_free(png_ptr, palette);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		SDL_RWclose(file);
		return false;
	}

	// Write header
	png_set_IHDR(
		png_ptr, info_ptr, dpi->width, dpi->height, 8,
		PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png_ptr, info_ptr);

	// Write pixels
	uint8 *bits = dpi->bits;
	for (int y = 0; y < dpi->height; y++) {
		png_write_row(png_ptr, (png_const_bytep)bits);
		bits += stride;
	}

	// Finish
	png_write_end(png_ptr, NULL);
	SDL_RWclose(file);

	png_free(png_ptr, palette);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return true;
#else
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

		b = RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8)[i * 4 + 0];
		g = RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8)[i * 4 + 1];
		r = RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8)[i * 4 + 2];

		lodepng_palette_add(&state.info_raw, r, g, b, a);
	}

	error = lodepng_encode(&png, &pngSize, dpi->bits, stride, dpi->height, &state);
	if (error != 0) {
		free(png);
		return false;
	} else {
		SDL_RWops *file = SDL_RWFromFile(path, "wb");
		if (file == NULL) {
			free(png);
			return false;
		}
		SDL_RWwrite(file, png, pngSize, 1);
		SDL_RWclose(file);
	}

	free(png);
	return true;
#endif
}

void screenshot_giant()
{
	int originalRotation = get_current_rotation();
	int originalZoom = 0;

	rct_window *mainWindow = window_get_main();
	if (mainWindow != NULL && mainWindow->viewport != NULL)
		originalZoom = mainWindow->viewport->zoom;

	int rotation = originalRotation;
	int zoom = originalZoom;
	int mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16);
	int resolutionWidth = (mapSize * 32 * 2) >> zoom;
	int resolutionHeight = (mapSize * 32 * 1) >> zoom;

	resolutionWidth += 8;
	resolutionHeight += 128;

	rct_viewport viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = resolutionWidth;
	viewport.height = resolutionHeight;
	viewport.view_width = viewport.width;
	viewport.view_height = viewport.height;
	viewport.var_11 = 0;
	viewport.flags = 0;

	int centreX = (mapSize / 2) * 32 + 16;
	int centreY = (mapSize / 2) * 32 + 16;

	int x, y;
	int z = map_element_height(centreX, centreY) & 0xFFFF;
	switch (rotation) {
	case 0:
		x = centreY - centreX;
		y = ((centreX + centreY) / 2) - z;
		break;
	case 1:
		x = -centreY - centreX;
		y = ((-centreX + centreY) / 2) - z;
		break;
	case 2:
		x = -centreY + centreX;
		y = ((-centreX - centreY) / 2) - z;
		break;
	case 3:
		x = centreY + centreX;
		y = ((centreX - centreY) / 2) - z;
		break;
	}

	viewport.view_x = x - ((viewport.view_width << zoom) / 2);
	viewport.view_y = y - ((viewport.view_height << zoom) / 2);
	viewport.zoom = zoom;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) = rotation;

	// Ensure sprites appear regardless of rotation
	reset_all_sprite_quadrant_placements();

	rct_drawpixelinfo dpi;
	dpi.x = 0;
	dpi.y = 0;
	dpi.width = resolutionWidth;
	dpi.height = resolutionHeight;
	dpi.pitch = 0;
	dpi.zoom_level = 0;
	dpi.bits = malloc(dpi.width * dpi.height);

	viewport_render(&dpi, &viewport, 0, 0, viewport.width, viewport.height);

	// Get a free screenshot path
	char path[MAX_PATH];
	int index;
	if ((index = screenshot_get_next_path(path, SCREENSHOT_FORMAT_PNG)) == -1) {
		log_error("Giant screenshot failed, unable to find a suitable destination path.");
		window_error_open(STR_SCREENSHOT_FAILED, -1);
		return;
	}

	screenshot_write_png(&dpi, path);

	free(dpi.bits);

	// Show user that screenshot saved successfully
	rct_string_id stringId = 3165;
	strcpy((char*)language_get_string(stringId), path_get_filename(path));
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = stringId;
	window_error_open(STR_SCREENSHOT_SAVED_AS, -1);
}

int cmdline_for_screenshot(const char **argv, int argc)
{
	bool giantScreenshot = argc == 5 && _stricmp(argv[2], "giant") == 0;
	if (argc != 4 && argc != 8 && !giantScreenshot) {
		printf("Usage: openrct2 screenshot <file> <ouput_image> <width> <height> [<x> <y> <zoom> <rotation>]\n");
		printf("Usage: openrct2 screenshot <file> <ouput_image> giant <zoom> <rotation>\n");
		return -1;
	}

	bool customLocation = false;
	bool centreMapX = false;
	bool centreMapY = false;
	int resolutionWidth, resolutionHeight, customX, customY, customZoom, customRotation;

	const char *inputPath = argv[0];
	const char *outputPath = argv[1];
	if (giantScreenshot) {
		resolutionWidth = 0;
		resolutionHeight = 0;
		customLocation = true;
		centreMapX = true;
		centreMapY = true;
		customZoom = atoi(argv[3]);
		customRotation = atoi(argv[4]) & 3;
	} else {
		resolutionWidth = atoi(argv[2]);
		resolutionHeight = atoi(argv[3]);
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
	}

	gOpenRCT2Headless = true;
	if (openrct2_initialise()) {
		rct2_open_file(inputPath);

		RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;

		int mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16);
		if (resolutionWidth == 0 || resolutionHeight == 0) {
			resolutionWidth = (mapSize * 32 * 2) >> customZoom;
			resolutionHeight = (mapSize * 32 * 1) >> customZoom;

			resolutionWidth += 8;
			resolutionHeight += 128;
		}

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
				customX = (mapSize / 2) * 32 + 16;
			if (centreMapY)
				customY = (mapSize / 2) * 32 + 16;

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

			viewport.view_x = x - ((viewport.view_width << customZoom) / 2);
			viewport.view_y = y - ((viewport.view_height << customZoom) / 2);
			viewport.zoom = customZoom;

			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) = customRotation;
		} else {
			viewport.view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16) - (viewport.view_width / 2);
			viewport.view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16) - (viewport.view_height / 2);
			viewport.zoom = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) & 0xFF;

			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_ZOOM_AND_ROTATION, uint16) >> 8;
		}

		// Ensure sprites appear regardless of rotation
		reset_all_sprite_quadrant_placements();

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

#ifdef USE_LIBPNG

static void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	SDL_RWops *file = (SDL_RWops*)png_get_io_ptr(png_ptr);
	SDL_RWwrite(file, data, length, 1);
}

static void my_png_flush(png_structp png_ptr)
{

}

#endif
