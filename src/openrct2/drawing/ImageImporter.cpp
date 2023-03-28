/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ImageImporter.h"

#include "../core/Imaging.h"

#include <cstring>
#include <stdexcept>
#include <string>

using namespace OpenRCT2::Drawing;
using ImportResult = ImageImporter::ImportResult;

constexpr int32_t PALETTE_TRANSPARENT = -1;

ImportResult ImageImporter::Import(
    const Image& image, int32_t offsetX, int32_t offsetY, Palette palette, ImportFlags flags, ImportMode mode) const
{
    return Import(image, 0, 0, image.Width, image.Height, offsetX, offsetY, palette, flags, mode);
}

ImportResult ImageImporter::Import(
    const Image& image, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t offsetX, int32_t offsetY,
    Palette palette, ImportFlags flags, ImportMode mode) const
{
    if (width > 256 || height > 256)
    {
        throw std::invalid_argument("Only images 256x256 or less are supported.");
    }

    if (palette == Palette::KeepIndices && image.Depth != 8)
    {
        throw std::invalid_argument("Image is not paletted, it has bit depth of " + std::to_string(image.Depth));
    }

    auto pixels = GetPixels(image.Pixels.data(), image.Stride, srcX, srcY, width, height, palette, flags, mode);
    auto buffer = flags & ImportFlags::RLE ? EncodeRLE(pixels.data(), width, height) : EncodeRaw(pixels.data(), width, height);

    G1Element outElement;
    outElement.width = width;
    outElement.height = height;
    outElement.flags = (flags & ImportFlags::RLE ? G1_FLAG_RLE_COMPRESSION : G1_FLAG_HAS_TRANSPARENCY);
    outElement.x_offset = offsetX;
    outElement.y_offset = offsetY;
    outElement.zoomed_offset = 0;

    ImportResult result;
    result.Element = outElement;
    result.Buffer = std::move(buffer);
    result.Element.offset = result.Buffer.data();
    return result;
}

std::vector<int32_t> ImageImporter::GetPixels(
    const uint8_t* pixels, uint32_t pitch, uint32_t srcX, uint32_t srcY, uint32_t width, uint32_t height, Palette palette,
    ImportFlags flags, ImportMode mode)
{
    std::vector<int32_t> buffer;
    buffer.reserve(width * height);

    // A larger range is needed for proper dithering
    auto palettedSrc = pixels;
    std::unique_ptr<int16_t[]> rgbaSrcBuffer;
    if (palette != Palette::KeepIndices)
    {
        rgbaSrcBuffer = std::make_unique<int16_t[]>(height * width * 4);
    }

    auto rgbaSrc = rgbaSrcBuffer.get();
    if (palette != Palette::KeepIndices)
    {
        auto src = pixels + (srcY * pitch) + (srcX * 4);
        auto dst = rgbaSrc;
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width * 4; x++)
            {
                *dst = static_cast<int16_t>(*src);
                src++;
                dst++;
            }
            src += (pitch - (width * 4));
        }
    }

    if (palette == Palette::KeepIndices)
    {
        palettedSrc += srcX + srcY * pitch;
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                int32_t paletteIndex = *palettedSrc;
                // The 1st index is always transparent
                if (paletteIndex == 0)
                {
                    paletteIndex = PALETTE_TRANSPARENT;
                }
                palettedSrc += 1;
                buffer.push_back(paletteIndex);
            }
            palettedSrc += (pitch - width);
        }
    }
    else
    {
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                auto paletteIndex = CalculatePaletteIndex(mode, rgbaSrc, x, y, width, height);
                rgbaSrc += 4;
                buffer.push_back(paletteIndex);
            }
        }
    }

    return buffer;
}

std::vector<uint8_t> ImageImporter::EncodeRaw(const int32_t* pixels, uint32_t width, uint32_t height)
{
    auto bufferLength = width * height;
    std::vector<uint8_t> buffer(bufferLength);
    for (size_t i = 0; i < bufferLength; i++)
    {
        auto p = pixels[i];
        buffer[i] = (p == PALETTE_TRANSPARENT ? 0 : static_cast<uint8_t>(p));
    }
    return buffer;
}

std::vector<uint8_t> ImageImporter::EncodeRLE(const int32_t* pixels, uint32_t width, uint32_t height)
{
    struct RLECode
    {
        uint8_t NumPixels{};
        uint8_t OffsetX{};
    };

    auto src = pixels;
    std::vector<uint8_t> buffer((height * 2) + (width * height * 16));

    std::fill_n(buffer.data(), (height * 2) + (width * height * 16), 0x00);
    auto yOffsets = reinterpret_cast<uint16_t*>(buffer.data());
    auto dst = buffer.data() + (height * 2);
    for (uint32_t y = 0; y < height; y++)
    {
        yOffsets[y] = static_cast<uint16_t>(dst - buffer.data());

        auto previousCode = static_cast<RLECode*>(nullptr);
        auto currentCode = reinterpret_cast<RLECode*>(dst);
        dst += 2;

        auto startX = 0;
        auto npixels = 0;
        bool pushRun = false;
        for (uint32_t x = 0; x < width; x++)
        {
            int32_t paletteIndex = *src++;
            if (paletteIndex == PALETTE_TRANSPARENT)
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
            if (npixels == 127 || x == width - 1)
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

                    if (x == width - 1)
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
        for (int32_t i = 0; i < PALETTE_SIZE; i++)
        {
            if (static_cast<int16_t>(palette[i].Red) == colour[0] && static_cast<int16_t>(palette[i].Green) == colour[1]
                && static_cast<int16_t>(palette[i].Blue) == colour[2])
            {
                return i;
            }
        }
    }
    return PALETTE_TRANSPARENT;
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
    return !(GetPaletteIndex(palette, colour) == PALETTE_TRANSPARENT && !IsTransparentPixel(colour));
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
    auto bestMatch = PALETTE_TRANSPARENT;
    for (int32_t x = 0; x < PALETTE_SIZE; x++)
    {
        if (IsChangablePixel(x))
        {
            uint32_t error = (static_cast<int16_t>(palette[x].Red) - colour[0])
                    * (static_cast<int16_t>(palette[x].Red) - colour[0])
                + (static_cast<int16_t>(palette[x].Green) - colour[1]) * (static_cast<int16_t>(palette[x].Green) - colour[1])
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
