#ifdef USE_LIBPNG
	#include <png.h>
#else
	#include <lodepng/lodepng.h>
#endif

#include "image_io.h"

#ifdef USE_LIBPNG
	static void my_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
	static void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
	static void my_png_flush(png_structp png_ptr);
#endif

bool image_io_png_read(uint8 **pixels, uint32 *width, uint32 *height, const utf8 *path)
{
#ifdef USE_LIBPNG
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;

	// Setup PNG structures
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		return false;
	}
 
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return false;
	}

	// Open PNG file
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	if (fp == NULL) {
		return false;
	}

	// Set error handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		SDL_RWclose(fp);
		return false;
	}
 
	// Setup png reading
	png_set_read_fn(png_ptr, fp, my_png_read_data);
	png_set_sig_bytes(png_ptr, sig_read);

	// To simplify the reading process, convert 4-16 bit data to 24-32 bit data
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
 
	// Read header
	png_uint_32 pngWidth, pngHeight;
	int bit_depth, color_type, interlace_type;
	png_get_IHDR(png_ptr, info_ptr, &pngWidth, &pngHeight, &bit_depth, &color_type, &interlace_type, NULL, NULL);

	// Read pixels as 32bpp RGBA data
	png_size_t rowBytes = png_get_rowbytes(png_ptr, info_ptr);
	png_bytepp rowPointers = png_get_rows(png_ptr, info_ptr);
	uint8 *pngPixels = (uint8*)malloc(pngWidth * pngHeight * 4);
	uint8 *dst = pngPixels;
	if (color_type == PNG_COLOR_TYPE_RGB) {
		// 24-bit PNG (no alpha)
		const png_size_t expectedRowSize = pngWidth * 3;
		for (png_uint_32 i = 0; i < pngHeight; i++) {
			assert(rowBytes == expectedRowSize);

			uint8 *src = rowPointers[i];
			for (png_uint_32 x = 0; x < pngWidth; x++) {
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = 255;
			}
		}
	} else {
		// 32-bit PNG (with alpha)
		const png_size_t expectedRowSize = pngWidth * 4;
		for (png_uint_32 i = 0; i < pngHeight; i++) {
			assert(rowBytes == expectedRowSize);

			memcpy(dst, rowPointers[i], rowBytes);
			dst += rowBytes;
		}
	}
 
	// Close the PNG
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	SDL_RWclose(fp);

	// Return the output data
	*pixels = (uint8*)pngPixels;
	if (width != NULL) *width = pngWidth;
	if (height != NULL) *height = pngHeight;
	return true;
#else
	// Read the pixels as 32bpp RGBA
	unsigned char *pngPixels;
	unsigned int pngWidth, pngHeight;
	unsigned int pngError = lodepng_decode_file(&pngPixels, &pngWidth, &pngHeight, path, LCT_RGBA, 8);
	if (pngError != 0) {
		free(pngPixels);
		log_error("Error creating PNG data, %u: %s", pngError, lodepng_error_text(pngError));
		return false;
	}

	// Return the output data
	*pixels = (uint8*)pngPixels;
	if (width != NULL) *width = pngWidth;
	if (height != NULL) *height = pngHeight;
	return true;
#endif
}

bool image_io_png_write(const rct_drawpixelinfo *dpi, const rct_palette *palette, const utf8 *path)
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

	png_colorp png_palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));
	for (int i = 0; i < 256; i++) {
		const rct_palette_entry *entry = &palette->entries[i];
		png_palette[i].blue		= entry->blue;
		png_palette[i].green	= entry->green;
		png_palette[i].red		= entry->red;
	}

	png_set_PLTE(png_ptr, info_ptr, png_palette, PNG_MAX_PALETTE_LENGTH);

	// Open file for writing
	SDL_RWops *file = SDL_RWFromFile(path, "wb");
	if (file == NULL) {
		png_free(png_ptr, png_palette);
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return false;
	}
	png_set_write_fn(png_ptr, file, my_png_write_data, my_png_flush);

	// Set error handler
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_free(png_ptr, png_palette);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		SDL_RWclose(file);
		return false;
	}

	// Write header
	png_set_IHDR(
		png_ptr, info_ptr, dpi->width, dpi->height, 8,
		PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
	);
	png_byte transparentIndex = 0;
	png_set_tRNS(png_ptr, info_ptr, &transparentIndex, 1, NULL);
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

	png_free(png_ptr, png_palette);
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
	int stride = dpi->width + dpi->pitch;

	lodepng_palette_add(&state.info_raw, 0, 0, 0, 0);
	for (int i = 1; i < 256; i++) {
		const rct_palette_entry *entry = &palette->entries[i];
		uint8 r = entry->red;
		uint8 g = entry->green;
		uint8 b = entry->blue;
		uint8 a = 255;
		lodepng_palette_add(&state.info_raw, r, g, b, a);
	}

	error = lodepng_encode(&png, &pngSize, dpi->bits, stride, dpi->height, &state);
	if (error != 0) {
		log_error("Error creating PNG data, %u: %s", error, lodepng_error_text(error));
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

#ifdef USE_LIBPNG

static void my_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	SDL_RWops *file = (SDL_RWops*)png_get_io_ptr(png_ptr);
	SDL_RWread(file, data, length, 1);
}

static void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	SDL_RWops *file = (SDL_RWops*)png_get_io_ptr(png_ptr);
	SDL_RWwrite(file, data, length, 1);
}

static void my_png_flush(png_structp png_ptr)
{

}

#endif

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
bool image_io_bmp_write(const rct_drawpixelinfo *dpi, const rct_palette *palette, const utf8 *path)
{
	BitmapFileHeader header;
	BitmapInfoHeader info;

	int i, y, width, height, stride;
	uint8 *buffer, *row;
	SDL_RWops *fp;
	unsigned int bytesWritten;

	// Open binary file for writing
	if ((fp = SDL_RWFromFile(path, "wb")) == NULL){
		return false;
	}

	// Allocate buffer
	buffer = malloc(0xFFFF);
	if (buffer == NULL) {
		SDL_RWclose(fp);
		return false;
	}

	// Get image size
	width = dpi->width;
	height = dpi->height;
	stride = dpi->width + dpi->pitch;

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
		return false;
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
		return false;
	}

	// Palette
	memset(buffer, 0, 246 * 4);
	for (i = 0; i < 246; i++) {
		const rct_palette_entry *entry = &palette->entries[i];
		buffer[i * 4 + 0] = entry->blue;
		buffer[i * 4 + 1] = entry->green;
		buffer[i * 4 + 2] = entry->red;
	}

	bytesWritten = SDL_RWwrite(fp, buffer, sizeof(char), 246 * 4);
	if (bytesWritten != 246*4){
		SDL_RWclose(fp);
		SafeFree(buffer);
		log_error("failed to save screenshot");
		return false;
	}

	// Image, save upside down
	for (y = dpi->height - 1; y >= 0; y--) {
		row = dpi->bits + y * (dpi->width + dpi->pitch);

		memset(buffer, 0, stride);
		memcpy(buffer, row, dpi->width);

		bytesWritten = SDL_RWwrite(fp, buffer, sizeof(char), stride);
		if (bytesWritten != stride){
			SDL_RWclose(fp);
			SafeFree(buffer);
			log_error("failed to save screenshot");
			return false;
		}
	}

	SDL_RWclose(fp);
	free(buffer);
	return true;
}
