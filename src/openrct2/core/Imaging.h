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

#include <memory>
#include <string_view>
#include <vector>
#include "../common.h"

struct rct_drawpixelinfo;
struct rct_palette;

enum class IMAGE_FORMAT
{
    UNKNOWN,
    AUTOMATIC,  // Automatically detect from file extension
    BITMAP,
    PNG,
    PNG_32,     // Force load to 32bpp buffer
};

struct Image
{
    // Meta
    uint32 Width{};
    uint32 Height{};
    uint32 Depth{};

    // Data
    std::vector<uint8> Pixels;
    std::unique_ptr<rct_palette> Palette;
    uint32 Stride{};
};


namespace Imaging
{
    Image ReadFromFile(const std::string_view& path, IMAGE_FORMAT format = IMAGE_FORMAT::AUTOMATIC);
    Image ReadFromBuffer(const std::vector<uint8>& buffer, IMAGE_FORMAT format = IMAGE_FORMAT::AUTOMATIC);
    void WriteToFile(const std::string_view& path, const Image& image, IMAGE_FORMAT format = IMAGE_FORMAT::AUTOMATIC);

    bool PngRead(uint8 * * pixels, uint32 * width, uint32 * height, bool expand, const utf8 * path, sint32 * bitDepth);
    bool PngWrite(const rct_drawpixelinfo * dpi, const rct_palette * palette, const utf8 * path);
    bool PngWrite32bpp(sint32 width, sint32 height, const void * pixels, const utf8 * path);
}
