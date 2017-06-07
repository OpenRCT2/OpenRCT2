#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#pragma once

#include "common.h"
#include "drawing/drawing.h"

#ifdef __cplusplus

namespace Imaging
{
    bool PngRead(uint8 * * pixels, uint32 * width, uint32 * height, const utf8 * path);
    bool PngWrite(const rct_drawpixelinfo * dpi, const rct_palette * palette, const utf8 * path);
    bool PngWrite32bpp(sint32 width, sint32 height, const void * pixels, const utf8 * path);
}

#endif // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif
    bool image_io_png_read(uint8 * * pixels, uint32 * width, uint32 * height, const utf8 * path);
    bool image_io_png_write(const rct_drawpixelinfo * dpi, const rct_palette * palette, const utf8 * path);
    bool image_io_png_write_32bpp(sint32 width, sint32 height, const void * pixels, const utf8 * path);
#ifdef __cplusplus
}
#endif
