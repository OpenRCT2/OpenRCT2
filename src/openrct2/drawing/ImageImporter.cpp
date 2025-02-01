/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ImageImporter.h"

#include "../core/Imaging.h"
#include "../core/Json.hpp"

#include <cstring>
#include <stdexcept>
#include <string>

namespace OpenRCT2::Drawing
{
    static constexpr int32_t kPaletteTransparent = -1;

    ImageImporter::ImportResult ImageImporter::Import(const Image& image, ImageImportMeta& meta) const
    {
        if (meta.srcSize.width == 0)
            meta.srcSize.width = image.Width;

        if (meta.srcSize.height == 0)
            meta.srcSize.height = image.Height;

        if (meta.srcSize.width > 256 || meta.srcSize.height > 256)
        {
            throw std::invalid_argument("Only images 256x256 or less are supported.");
        }

        if (meta.palette == Palette::KeepIndices && image.Depth != 8)
        {
            throw std::invalid_argument("Image is not paletted, it has bit depth of " + std::to_string(image.Depth));
        }
        const bool isRLE = HasFlag(meta.importFlags, ImportFlags::RLE);

        auto pixels = GetPixels(image, meta);
        auto buffer = isRLE ? EncodeRLE(pixels.data(), meta.srcSize) : EncodeRaw(pixels.data(), meta.srcSize);

        G1Element outElement;
        outElement.width = meta.srcSize.width;
        outElement.height = meta.srcSize.height;
        outElement.flags = isRLE ? G1_FLAG_RLE_COMPRESSION : G1_FLAG_HAS_TRANSPARENCY;
        outElement.x_offset = meta.offset.x;
        outElement.y_offset = meta.offset.y;
        outElement.zoomed_offset = meta.zoomedOffset;
        if (HasFlag(meta.importFlags, ImportFlags::NoDrawOnZoom))
            outElement.flags |= G1_FLAG_NO_ZOOM_DRAW;

        ImageImporter::ImportResult result;
        result.Element = outElement;
        result.Buffer = std::move(buffer);
        result.Element.offset = result.Buffer.data();
        return result;
    }

    std::vector<int32_t> ImageImporter::GetPixels(const Image& image, const ImageImportMeta& meta)
    {
        const uint8_t* pixels = image.Pixels.data();
        std::vector<int32_t> buffer;
        buffer.reserve(meta.srcSize.width * meta.srcSize.height);

        // A larger range is needed for proper dithering
        auto palettedSrc = pixels;
        std::unique_ptr<int16_t[]> rgbaSrcBuffer;
        if (meta.palette != Palette::KeepIndices)
        {
            rgbaSrcBuffer = std::make_unique<int16_t[]>(meta.srcSize.height * meta.srcSize.width * 4);
        }

        auto rgbaSrc = rgbaSrcBuffer.get();
        if (meta.palette != Palette::KeepIndices)
        {
            auto src = pixels + (meta.srcOffset.y * image.Stride) + (meta.srcOffset.x * 4);
            auto dst = rgbaSrc;
            for (auto y = 0; y < meta.srcSize.height; y++)
            {
                for (auto x = 0; x < meta.srcSize.width * 4; x++)
                {
                    *dst = static_cast<int16_t>(*src);
                    src++;
                    dst++;
                }
                src += (image.Stride - (meta.srcSize.width * 4));
            }
        }

        if (meta.palette == Palette::KeepIndices)
        {
            palettedSrc += meta.srcOffset.x + meta.srcOffset.y * image.Stride;
            for (auto y = 0; y < meta.srcSize.height; y++)
            {
                for (auto x = 0; x < meta.srcSize.width; x++)
                {
                    int32_t paletteIndex = *palettedSrc;
                    // The 1st index is always transparent
                    if (paletteIndex == 0)
                    {
                        paletteIndex = kPaletteTransparent;
                    }
                    palettedSrc += 1;
                    buffer.push_back(paletteIndex);
                }
                palettedSrc += (image.Stride - meta.srcSize.width);
            }
        }
        else
        {
            for (auto y = 0; y < meta.srcSize.height; y++)
            {
                for (auto x = 0; x < meta.srcSize.width; x++)
                {
                    auto paletteIndex = CalculatePaletteIndex(
                        meta.importMode, rgbaSrc, x, y, meta.srcSize.width, meta.srcSize.height);
                    rgbaSrc += 4;
                    buffer.push_back(paletteIndex);
                }
            }
        }

        return buffer;
    }

    std::vector<uint8_t> ImageImporter::EncodeRaw(const int32_t* pixels, ScreenSize size)
    {
        auto bufferLength = size.width * size.height;
        std::vector<uint8_t> buffer(bufferLength);
        for (auto i = 0; i < bufferLength; i++)
        {
            auto p = pixels[i];
            buffer[i] = (p == kPaletteTransparent ? 0 : static_cast<uint8_t>(p));
        }
        return buffer;
    }

    std::vector<uint8_t> ImageImporter::EncodeRLE(const int32_t* pixels, ScreenSize size)
    {
        struct RLECode
        {
            uint8_t NumPixels{};
            uint8_t OffsetX{};
        };

        auto src = pixels;
        std::vector<uint8_t> buffer((size.height * 2) + (size.width * size.height * 16));

        std::fill_n(buffer.data(), (size.height * 2) + (size.width * size.height * 16), 0x00);
        auto yOffsets = reinterpret_cast<uint16_t*>(buffer.data());
        auto dst = buffer.data() + (size.height * 2);
        for (auto y = 0; y < size.height; y++)
        {
            yOffsets[y] = static_cast<uint16_t>(dst - buffer.data());

            auto previousCode = static_cast<RLECode*>(nullptr);
            auto currentCode = reinterpret_cast<RLECode*>(dst);
            dst += 2;

            auto startX = 0;
            auto npixels = 0;
            bool pushRun = false;
            for (auto x = 0; x < size.width; x++)
            {
                int32_t paletteIndex = *src++;
                if (paletteIndex == kPaletteTransparent)
                {
                    if (npixels != 0)
                    {
                        x--;
                        src--;
                        pushRun = true;
                    }
                }
                else
                {
                    if (npixels == 0)
                    {
                        startX = x;
                    }

                    npixels++;
                    *dst++ = static_cast<uint8_t>(paletteIndex);
                }
                if (npixels == 127 || x == size.width - 1)
                {
                    pushRun = true;
                }

                if (pushRun)
                {
                    if (npixels > 0)
                    {
                        previousCode = currentCode;
                        currentCode->NumPixels = npixels;
                        currentCode->OffsetX = startX;

                        if (x == size.width - 1)
                        {
                            currentCode->NumPixels |= 0x80;
                        }

                        currentCode = reinterpret_cast<RLECode*>(dst);
                        dst += 2;
                    }
                    else
                    {
                        if (previousCode == nullptr)
                        {
                            currentCode->NumPixels = 0x80;
                            currentCode->OffsetX = 0;
                        }
                        else
                        {
                            previousCode->NumPixels |= 0x80;
                            dst -= 2;
                        }
                    }

                    startX = 0;
                    npixels = 0;
                    pushRun = false;
                }
            }
        }

        auto bufferLength = static_cast<size_t>(dst - buffer.data());
        buffer.resize(bufferLength);
        return buffer;
    }

    int32_t ImageImporter::CalculatePaletteIndex(
        ImportMode mode, int16_t* rgbaSrc, int32_t x, int32_t y, int32_t width, int32_t height)
    {
        auto& palette = StandardPalette;
        auto paletteIndex = GetPaletteIndex(palette, rgbaSrc);
        if ((mode == ImportMode::Closest || mode == ImportMode::Dithering) && !IsInPalette(palette, rgbaSrc))
        {
            paletteIndex = GetClosestPaletteIndex(palette, rgbaSrc);
            if (mode == ImportMode::Dithering)
            {
                auto dr = rgbaSrc[0] - static_cast<int16_t>(palette[paletteIndex].Red);
                auto dg = rgbaSrc[1] - static_cast<int16_t>(palette[paletteIndex].Green);
                auto db = rgbaSrc[2] - static_cast<int16_t>(palette[paletteIndex].Blue);

                // We don't want to dither remappable colours with nonremappable colours, etc
                PaletteIndexType thisIndexType = GetPaletteIndexType(paletteIndex);

                if (x + 1 < width)
                {
                    if (!IsInPalette(palette, rgbaSrc + 4)
                        && thisIndexType == GetPaletteIndexType(GetClosestPaletteIndex(palette, rgbaSrc + 4)))
                    {
                        // Right
                        rgbaSrc[4] += dr * 7 / 16;
                        rgbaSrc[5] += dg * 7 / 16;
                        rgbaSrc[6] += db * 7 / 16;
                    }
                }

                if (y + 1 < height)
                {
                    if (x > 0)
                    {
                        if (!IsInPalette(palette, rgbaSrc + 4 * (width - 1))
                            && thisIndexType == GetPaletteIndexType(GetClosestPaletteIndex(palette, rgbaSrc + 4 * (width - 1))))
                        {
                            // Bottom left
                            rgbaSrc[4 * (width - 1)] += dr * 3 / 16;
                            rgbaSrc[4 * (width - 1) + 1] += dg * 3 / 16;
                            rgbaSrc[4 * (width - 1) + 2] += db * 3 / 16;
                        }
                    }

                    // Bottom
                    if (!IsInPalette(palette, rgbaSrc + 4 * width)
                        && thisIndexType == GetPaletteIndexType(GetClosestPaletteIndex(palette, rgbaSrc + 4 * width)))
                    {
                        rgbaSrc[4 * width] += dr * 5 / 16;
                        rgbaSrc[4 * width + 1] += dg * 5 / 16;
                        rgbaSrc[4 * width + 2] += db * 5 / 16;
                    }

                    if (x + 1 < width)
                    {
                        if (!IsInPalette(palette, rgbaSrc + 4 * (width + 1))
                            && thisIndexType == GetPaletteIndexType(GetClosestPaletteIndex(palette, rgbaSrc + 4 * (width + 1))))
                        {
                            // Bottom right
                            rgbaSrc[4 * (width + 1)] += dr * 1 / 16;
                            rgbaSrc[4 * (width + 1) + 1] += dg * 1 / 16;
                            rgbaSrc[4 * (width + 1) + 2] += db * 1 / 16;
                        }
                    }
                }
            }
        }

        return paletteIndex;
    }

    int32_t ImageImporter::GetPaletteIndex(const GamePalette& palette, int16_t* colour)
    {
        if (!IsTransparentPixel(colour))
        {
            for (uint32_t i = 0; i < kGamePaletteSize; i++)
            {
                if (static_cast<int16_t>(palette[i].Red) == colour[0] && static_cast<int16_t>(palette[i].Green) == colour[1]
                    && static_cast<int16_t>(palette[i].Blue) == colour[2])
                {
                    return i;
                }
            }
        }
        return kPaletteTransparent;
    }

    bool ImageImporter::IsTransparentPixel(const int16_t* colour)
    {
        return colour[3] < 128;
    }

    /**
     * @returns true if this colour is in the standard palette.
     */
    bool ImageImporter::IsInPalette(const GamePalette& palette, int16_t* colour)
    {
        return !(GetPaletteIndex(palette, colour) == kPaletteTransparent && !IsTransparentPixel(colour));
    }

    /**
     * @returns true if palette index is an index not used for a special purpose.
     */
    bool ImageImporter::IsChangablePixel(int32_t paletteIndex)
    {
        PaletteIndexType entryType = GetPaletteIndexType(paletteIndex);
        return entryType != PaletteIndexType::Special && entryType != PaletteIndexType::PrimaryRemap;
    }

    /**
     * @returns the type of palette entry this is.
     */
    ImageImporter::PaletteIndexType ImageImporter::GetPaletteIndexType(int32_t paletteIndex)
    {
        if (paletteIndex <= 9)
            return PaletteIndexType::Special;
        if (paletteIndex >= 230 && paletteIndex <= 239)
            return PaletteIndexType::Special;
        if (paletteIndex == 255)
            return PaletteIndexType::Special;
        if (paletteIndex >= 243 && paletteIndex <= 254)
            return PaletteIndexType::PrimaryRemap;
        if (paletteIndex >= 202 && paletteIndex <= 213)
            return PaletteIndexType::SecondaryRemap;
        if (paletteIndex >= 46 && paletteIndex <= 57)
            return PaletteIndexType::TertiaryRemap;
        return PaletteIndexType::Normal;
    }

    int32_t ImageImporter::GetClosestPaletteIndex(const GamePalette& palette, const int16_t* colour)
    {
        auto smallestError = static_cast<uint32_t>(-1);
        auto bestMatch = kPaletteTransparent;
        for (uint32_t x = 0; x < kGamePaletteSize; x++)
        {
            if (IsChangablePixel(x))
            {
                uint32_t error = (static_cast<int16_t>(palette[x].Red) - colour[0])
                        * (static_cast<int16_t>(palette[x].Red) - colour[0])
                    + (static_cast<int16_t>(palette[x].Green) - colour[1])
                        * (static_cast<int16_t>(palette[x].Green) - colour[1])
                    + (static_cast<int16_t>(palette[x].Blue) - colour[2]) * (static_cast<int16_t>(palette[x].Blue) - colour[2]);

                if (smallestError == static_cast<uint32_t>(-1) || smallestError > error)
                {
                    bestMatch = x;
                    smallestError = error;
                }
            }
        }
        return bestMatch;
    }

    ImageImportMeta createImageImportMetaFromJson(json_t& input)
    {
        auto xOffset = Json::GetNumber<int16_t>(input["x"]);
        auto yOffset = Json::GetNumber<int16_t>(input["y"]);
        auto keepPalette = Json::GetString(input["palette"]) == "keep";
        auto palette = keepPalette ? Palette::KeepIndices : Palette::OpenRCT2;
        uint8_t flags = 0;

        auto raw = Json::GetString(input["format"]) == "raw";
        if (!raw)
            flags |= EnumToFlag(ImportFlags::RLE);

        if (Json::GetBoolean("noDrawOnZoom"))
            flags |= EnumToFlag(ImportFlags::NoDrawOnZoom);

        auto srcX = Json::GetNumber<int16_t>(input["srcX"]);
        auto srcY = Json::GetNumber<int16_t>(input["srcY"]);
        auto srcWidth = Json::GetNumber<int16_t>(input["srcWidth"]);
        auto srcHeight = Json::GetNumber<int16_t>(input["srcHeight"]);
        auto zoomedOffset = Json::GetNumber<int32_t>(input["zoom"]);

        return ImageImportMeta{ { xOffset, yOffset },    palette,     flags, ImportMode::Default, { srcX, srcY },
                                { srcWidth, srcHeight }, zoomedOffset };
    };
} // namespace OpenRCT2::Drawing
