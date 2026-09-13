/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../drawing/ColourPalette.h"

#include <functional>
#include <istream>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

enum class ImageFormat
{
    unknown,
    automatic, // Automatically detect from file extension
    bitmap,
    png,
    png32, // Force load to 32bpp buffer
};

struct Image
{
    // Meta
    uint32_t Width{};
    uint32_t Height{};
    uint32_t Depth{};

    // Data
    std::vector<uint8_t> Pixels;
    std::optional<OpenRCT2::Drawing::GamePalette> Palette;
    uint32_t Stride{};
};

using ImageReaderFunc = std::function<Image(std::istream&, ImageFormat)>;

namespace OpenRCT2::Imaging
{
    ImageFormat GetImageFormatFromPath(std::string_view path);
    Image ReadFromFile(std::string_view path, ImageFormat format = ImageFormat::automatic);
    Image ReadFromBuffer(const std::vector<uint8_t>& buffer, ImageFormat format = ImageFormat::automatic);
    void WriteToFile(std::string_view path, const Image& image, ImageFormat format = ImageFormat::automatic);

    void SetReader(ImageFormat format, ImageReaderFunc impl);
} // namespace OpenRCT2::Imaging
