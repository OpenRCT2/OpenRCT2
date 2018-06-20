/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <functional>
#include <istream>
#include <memory>
#include <string_view>
#include <vector>
#include "../common.h"
#include "../drawing/Drawing.h"

struct rct_drawpixelinfo;
struct rct_palette;

struct PaletteBGRA
{
    uint8_t Blue{};
    uint8_t Green{};
    uint8_t Red{};
    uint8_t Alpha{};
};

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
    uint32_t Width{};
    uint32_t Height{};
    uint32_t Depth{};

    // Data
    std::vector<uint8_t> Pixels;
    std::unique_ptr<rct_palette> Palette;
    uint32_t Stride{};
};

using ImageReaderFunc = std::function<Image(std::istream&, IMAGE_FORMAT)>;

namespace Imaging
{
    IMAGE_FORMAT GetImageFormatFromPath(const std::string_view& path);
    Image ReadFromFile(const std::string_view& path, IMAGE_FORMAT format = IMAGE_FORMAT::AUTOMATIC);
    Image ReadFromBuffer(const std::vector<uint8_t>& buffer, IMAGE_FORMAT format = IMAGE_FORMAT::AUTOMATIC);
    void WriteToFile(const std::string_view& path, const Image& image, IMAGE_FORMAT format = IMAGE_FORMAT::AUTOMATIC);

    void SetReader(IMAGE_FORMAT format, ImageReaderFunc impl);
}
