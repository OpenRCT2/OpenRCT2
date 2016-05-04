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

#ifndef _IMAGE_IO_H_
#define _IMAGE_IO_H_

#include "common.h"
#include "drawing/drawing.h"

bool image_io_png_read(uint8 **pixels, uint32 *width, uint32 *height, const utf8 *path);

bool image_io_png_write(const rct_drawpixelinfo *dpi, const rct_palette *palette, const utf8 *path);
bool image_io_bmp_write(const rct_drawpixelinfo *dpi, const rct_palette *palette, const utf8 *path);

#endif
