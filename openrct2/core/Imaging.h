/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../drawing/Drawing.h"

#include <functional>
#include <istream>
#include <memory>
#include <string_view>
#include <vector>

struct rct_drawpixelinfo;

enum class IMAGE_FORMAT
{
    UNKNOWN,
    AUTOMATIC, // Automatically detect from file extension
    BITMAP,
    PNG,
    PNG_32, // Force load to 32bpp buffer
};

struct Image
{
    // Meta
    uint32_t Width{};
    uint32_t Height{};
    uint32_t Depth{};

    // Data
    std::vector<uint8_t> Pixels;
    std::unique_ptr<GamePalette> Palette;
    uint32_t Stride{};
};

using ImageReaderFunc = std::function<Image(std::istream&, IMAGE_FORMAT)>;

namespace Imaging
{
    IMAGE_FORMAT GetImageFormatFromPath(std::string_view path);
    Image ReadFromFile(std::string_view path, IMAGE_FORMAT format = IMAGE_FORMAT::AUTOMATIC);
    Image ReadFromBuffer(const std::vector<uint8_t>& buffer, IMAGE_FORMAT format = IMAGE_FORMAT::AUTOMATIC);
    void WriteToFile(std::string_view path, const Image& image, IMAGE_FORMAT format = IMAGE_FORMAT::AUTOMATIC);

    void SetReader(IMAGE_FORMAT format, ImageReaderFunc impl);
} // namespace Imaging
