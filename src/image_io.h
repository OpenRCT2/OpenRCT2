#ifndef _IMAGE_IO_H_
#define _IMAGE_IO_H_

#include "common.h"
#include "drawing/drawing.h"

bool image_io_png_read(uint8 **pixels, uint32 *width, uint32 *height, const utf8 *path);

bool image_io_png_write(const rct_drawpixelinfo *dpi, const rct_palette *palette, const utf8 *path);
bool image_io_bmp_write(const rct_drawpixelinfo *dpi, const rct_palette *palette, const utf8 *path);

#endif
