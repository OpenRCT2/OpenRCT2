/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cstring>
#include <stdexcept>
#include <string>
#include "../core/Imaging.h"
#include "ImageImporter.h"

using namespace OpenRCT2::Drawing;
using ImportResult = ImageImporter::ImportResult;

constexpr int32_t PALETTE_TRANSPARENT = -1;

ImportResult ImageImporter::Import(
    const Image& image,
    int32_t offsetX,
    int32_t offsetY,
    IMPORT_FLAGS flags,
    IMPORT_MODE mode) const
{
    if (image.Width > 256 || image.Height > 256)
    {
        throw std::invalid_argument("Only images 256x256 or less are supported.");
    }

    if ((flags & IMPORT_FLAGS::KEEP_PALETTE) && image.Depth != 8)
    {
        throw std::invalid_argument("Image is not palletted, it has bit depth of " + std::to_string(image.Depth));
    }

    const auto width = image.Width;
    const auto height = image.Height;

    auto pixels = GetPixels(image.Pixels.data(), width, height, flags, mode);
    auto [buffer, bufferLength] = flags & IMPORT_FLAGS::RLE ?
        EncodeRLE(pixels.data(), width, height) :
        EncodeRaw(pixels.data(), width, height);

    rct_g1_element outElement;
    outElement.offset = (uint8_t *)buffer;
    outElement.width = width;
    outElement.height = height;
    outElement.flags = (flags & IMPORT_FLAGS::RLE ? G1_FLAG_RLE_COMPRESSION : G1_FLAG_BMP);
    outElement.x_offset = offsetX;
    outElement.y_offset = offsetY;
    outElement.zoomed_offset = 0;

    ImportResult result;
    result.Element = outElement;
    result.Buffer = buffer;
    result.BufferLength = bufferLength;
    return result;
}

std::vector<int32_t> ImageImporter::GetPixels(const uint8_t * pixels, uint32_t width, uint32_t height, IMPORT_FLAGS flags, IMPORT_MODE mode)
{
    std::vector<int32_t> buffer;
    buffer.reserve(width * height);

    // A larger range is needed for proper dithering
    auto palettedSrc = pixels;
    std::unique_ptr<int16_t[]> rgbaSrcBuffer;
    if (!(flags & IMPORT_FLAGS::KEEP_PALETTE))
    {
        rgbaSrcBuffer = std::make_unique<int16_t[]>(height * width * 4);
    }

    auto rgbaSrc = rgbaSrcBuffer.get();
    if (!(flags & IMPORT_FLAGS::KEEP_PALETTE))
    {
        for (uint32_t x = 0; x < height * width * 4; x++)
        {
            rgbaSrc[x] = (int16_t)pixels[x];
        }
    }

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            int32_t paletteIndex;
            if (flags & IMPORT_FLAGS::KEEP_PALETTE)
            {
                paletteIndex = *palettedSrc;
                // The 1st index is always transparent
                if (paletteIndex == 0)
                {
                    paletteIndex = PALETTE_TRANSPARENT;
                }
            }
            else
            {
                paletteIndex = CalculatePaletteIndex(mode, rgbaSrc, x, y, width, height);
            }

            rgbaSrc += 4;
            palettedSrc += 1;

            buffer.push_back(paletteIndex);
        }
    }

    return buffer;
}

std::tuple<void *, size_t> ImageImporter::EncodeRaw(const int32_t * pixels, uint32_t width, uint32_t height)
{
    auto bufferLength = width * height;
    auto buffer = (uint8_t *)std::malloc(bufferLength);
    for (size_t i = 0; i < bufferLength; i++)
    {
        auto p = pixels[i];
        buffer[i] = (p == PALETTE_TRANSPARENT ? 0 : (uint8_t)p);
    }
    return std::make_tuple(buffer, bufferLength);
}

std::tuple<void *, size_t> ImageImporter::EncodeRLE(const int32_t * pixels, uint32_t width, uint32_t height)
{
    struct RLECode
    {
        uint8_t NumPixels{};
        uint8_t OffsetX{};
    };

    auto src = pixels;
    auto buffer = (uint8_t *)std::malloc((height * 2) + (width * height * 16));
    if (buffer == nullptr)
    {
        throw std::bad_alloc();
    }

    std::memset(buffer, 0, (height * 2) + (width * height * 16));
    auto yOffsets = (uint16_t *)buffer;
    auto dst = buffer + (height * 2);
    for (uint32_t y = 0; y < height; y++)
    {
        yOffsets[y] = (uint16_t)(dst - buffer);

        auto previousCode = (RLECode *)nullptr;
        auto currentCode = (RLECode *)dst;
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
                *dst++ = (uint8_t)paletteIndex;
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

                    currentCode = (RLECode *)dst;
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

    auto bufferLength = (size_t)(dst - buffer);
    buffer = (uint8_t * )realloc(buffer, bufferLength);
    if (buffer == nullptr)
    {
        throw std::bad_alloc();
    }
    return std::make_tuple(buffer, bufferLength);
}

int32_t ImageImporter::CalculatePaletteIndex(IMPORT_MODE mode, int16_t * rgbaSrc, int32_t x, int32_t y, int32_t width, int32_t height)
{
    auto palette = StandardPalette;
    auto paletteIndex = GetPaletteIndex(palette, rgbaSrc);
    if (mode == IMPORT_MODE::CLOSEST || mode == IMPORT_MODE::DITHERING)
    {
        if (paletteIndex == PALETTE_TRANSPARENT && !IsTransparentPixel(rgbaSrc))
        {
            paletteIndex = GetClosestPaletteIndex(palette, rgbaSrc);
        }
    }
    if (mode == IMPORT_MODE::DITHERING)
    {
        if (!IsTransparentPixel(rgbaSrc) && IsChangablePixel(GetPaletteIndex(palette, rgbaSrc)))
        {
            auto dr = rgbaSrc[0] - (int16_t)(palette[paletteIndex].Red);
            auto dg = rgbaSrc[1] - (int16_t)(palette[paletteIndex].Green);
            auto db = rgbaSrc[2] - (int16_t)(palette[paletteIndex].Blue);

            if (x + 1 < width)
            {
                if (!IsTransparentPixel(rgbaSrc + 4) && IsChangablePixel(GetPaletteIndex(palette, rgbaSrc + 4)))
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
                    if (!IsTransparentPixel(rgbaSrc + 4 * (width - 1)) && IsChangablePixel(GetPaletteIndex(palette, rgbaSrc + 4 * (width - 1))))
                    {
                        // Bottom left
                        rgbaSrc[4 * (width - 1)] += dr * 3 / 16;
                        rgbaSrc[4 * (width - 1) + 1] += dg * 3 / 16;
                        rgbaSrc[4 * (width - 1) + 2] += db * 3 / 16;
                    }
                }

                // Bottom
                if (!IsTransparentPixel(rgbaSrc + 4 * width) && IsChangablePixel(GetPaletteIndex(palette, rgbaSrc + 4 * width)))
                {
                    rgbaSrc[4 * width] += dr * 5 / 16;
                    rgbaSrc[4 * width + 1] += dg * 5 / 16;
                    rgbaSrc[4 * width + 2] += db * 5 / 16;
                }

                if (x + 1 < width)
                {
                    if (!IsTransparentPixel(rgbaSrc + 4 * (width + 1)) && IsChangablePixel(GetPaletteIndex(palette, rgbaSrc + 4 * (width + 1))))
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

int32_t ImageImporter::GetPaletteIndex(const PaletteBGRA * palette, int16_t * colour)
{
    if (!IsTransparentPixel(colour))
    {
        for (int32_t i = 0; i < 256; i++)
        {
            if ((int16_t)(palette[i].Red) == colour[0] &&
                (int16_t)(palette[i].Green) == colour[1] &&
                (int16_t)(palette[i].Blue) == colour[2])
            {
                return i;
            }
        }
    }
    return PALETTE_TRANSPARENT;
}

bool ImageImporter::IsTransparentPixel(const int16_t * colour)
{
    return colour[3] < 128;
}

/**
 * @returns true if pixel index is an index not used for remapping.
 */
bool ImageImporter::IsChangablePixel(int32_t paletteIndex)
{
    if (paletteIndex == PALETTE_TRANSPARENT)
        return true;
    if (paletteIndex == 0)
        return false;
    if (paletteIndex >= 203 && paletteIndex < 214)
        return false;
    if (paletteIndex == 226)
        return false;
    if (paletteIndex >= 227 && paletteIndex < 229)
        return false;
    if (paletteIndex >= 243)
        return false;
    return true;
}

int32_t ImageImporter::GetClosestPaletteIndex(const PaletteBGRA * palette, const int16_t * colour)
{
    auto smallestError = (uint32_t)-1;
    auto bestMatch = PALETTE_TRANSPARENT;
    for (int32_t x = 0; x < 256; x++)
    {
        if (IsChangablePixel(x))
        {
            uint32_t error =
                ((int16_t)(palette[x].Red) - colour[0]) * ((int16_t)(palette[x].Red) - colour[0]) +
                ((int16_t)(palette[x].Green) - colour[1]) * ((int16_t)(palette[x].Green) - colour[1]) +
                ((int16_t)(palette[x].Blue) - colour[2]) * ((int16_t)(palette[x].Blue) - colour[2]);

            if (smallestError == (uint32_t)-1 || smallestError > error)
            {
                bestMatch = x;
                smallestError = error;
            }
        }
    }
    return bestMatch;
}

const PaletteBGRA ImageImporter::StandardPalette[256] =
{
    // 0 (unused)
    { 0, 0, 0, 255 },

    // 1 - 9 (misc. e.g. font and water)
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },

    //
    { 35, 35, 23, 255 },
    { 51, 51, 35, 255 },
    { 67, 67, 47, 255 },
    { 83, 83, 63, 255 },
    { 99, 99, 75, 255 },
    { 115, 115, 91, 255 },
    { 131, 131, 111, 255 },
    { 151, 151, 131, 255 },
    { 175, 175, 159, 255 },
    { 195, 195, 183, 255 },
    { 219, 219, 211, 255 },
    { 243, 243, 239, 255 },
    { 0, 47, 51, 255 },
    { 0, 59, 63, 255 },
    { 11, 75, 79, 255 },
    { 19, 91, 91, 255 },
    { 31, 107, 107, 255 },
    { 47, 123, 119, 255 },
    { 59, 139, 135, 255 },
    { 79, 155, 151, 255 },
    { 95, 175, 167, 255 },
    { 115, 191, 187, 255 },
    { 139, 207, 203, 255 },
    { 163, 227, 223, 255 },
    { 7, 43, 67, 255 },
    { 11, 59, 87, 255 },
    { 23, 75, 111, 255 },
    { 31, 87, 127, 255 },
    { 39, 99, 143, 255 },
    { 51, 115, 159, 255 },
    { 67, 131, 179, 255 },
    { 87, 151, 191, 255 },
    { 111, 175, 203, 255 },
    { 135, 199, 219, 255 },
    { 163, 219, 231, 255 },
    { 195, 239, 247, 255 },
    { 0, 27, 71, 255 },
    { 0, 43, 95, 255 },
    { 0, 63, 119, 255 },
    { 7, 83, 143, 255 },
    { 7, 111, 167, 255 },
    { 15, 139, 191, 255 },
    { 19, 167, 215, 255 },
    { 27, 203, 243, 255 },
    { 47, 231, 255, 255 },
    { 95, 243, 255, 255 },
    { 143, 251, 255, 255 },
    { 195, 255, 255, 255 },
    { 0, 0, 35, 255 },
    { 0, 0, 79, 255 },
    { 7, 7, 95, 255 },
    { 15, 15, 111, 255 },
    { 27, 27, 127, 255 },
    { 39, 39, 143, 255 },
    { 59, 59, 163, 255 },
    { 79, 79, 179, 255 },
    { 103, 103, 199, 255 },
    { 127, 127, 215, 255 },
    { 159, 159, 235, 255 },
    { 191, 191, 255, 255 },
    { 19, 51, 27, 255 },
    { 23, 63, 35, 255 },
    { 31, 79, 47, 255 },
    { 39, 95, 59, 255 },
    { 43, 111, 71, 255 },
    { 51, 127, 87, 255 },
    { 59, 143, 99, 255 },
    { 67, 155, 115, 255 },
    { 75, 171, 131, 255 },
    { 83, 187, 147, 255 },
    { 95, 203, 163, 255 },
    { 103, 219, 183, 255 },
    { 27, 55, 31, 255 },
    { 35, 71, 47, 255 },
    { 43, 83, 59, 255 },
    { 55, 99, 75, 255 },
    { 67, 111, 91, 255 },
    { 79, 135, 111, 255 },
    { 95, 159, 135, 255 },
    { 111, 183, 159, 255 },
    { 127, 207, 183, 255 },
    { 147, 219, 195, 255 },
    { 167, 231, 207, 255 },
    { 191, 247, 223, 255 },
    { 0, 63, 15, 255 },
    { 0, 83, 19, 255 },
    { 0, 103, 23, 255 },
    { 0, 123, 31, 255 },
    { 7, 143, 39, 255 },
    { 23, 159, 55, 255 },
    { 39, 175, 71, 255 },
    { 63, 191, 91, 255 },
    { 87, 207, 111, 255 },
    { 115, 223, 139, 255 },
    { 143, 239, 163, 255 },
    { 179, 255, 195, 255 },
    { 19, 43, 79, 255 },
    { 27, 55, 99, 255 },
    { 43, 71, 119, 255 },
    { 59, 87, 139, 255 },
    { 67, 99, 167, 255 },
    { 83, 115, 187, 255 },
    { 99, 131, 207, 255 },
    { 115, 151, 215, 255 },
    { 131, 171, 227, 255 },
    { 151, 191, 239, 255 },
    { 171, 207, 247, 255 },
    { 195, 227, 255, 255 },
    { 55, 19, 15, 255 },
    { 87, 43, 39, 255 },
    { 103, 55, 51, 255 },
    { 119, 67, 63, 255 },
    { 139, 83, 83, 255 },
    { 155, 99, 99, 255 },
    { 175, 119, 119, 255 },
    { 191, 139, 139, 255 },
    { 207, 159, 159, 255 },
    { 223, 183, 183, 255 },
    { 239, 211, 211, 255 },
    { 255, 239, 239, 255 },
    { 111, 27, 0, 255 },
    { 151, 39, 0, 255 },
    { 167, 51, 7, 255 },
    { 187, 67, 15, 255 },
    { 203, 83, 27, 255 },
    { 223, 103, 43, 255 },
    { 227, 135, 67, 255 },
    { 231, 163, 91, 255 },
    { 239, 187, 119, 255 },
    { 243, 211, 143, 255 },
    { 251, 231, 175, 255 },
    { 255, 247, 215, 255 },
    { 15, 43, 11, 255 },
    { 23, 55, 15, 255 },
    { 31, 71, 23, 255 },
    { 43, 83, 35, 255 },
    { 59, 99, 47, 255 },
    { 75, 115, 59, 255 },
    { 95, 135, 79, 255 },
    { 119, 155, 99, 255 },
    { 139, 175, 123, 255 },
    { 167, 199, 147, 255 },
    { 195, 219, 175, 255 },
    { 223, 243, 207, 255 },
    { 95, 0, 63, 255 },
    { 115, 7, 75, 255 },
    { 127, 15, 83, 255 },
    { 143, 31, 95, 255 },
    { 155, 43, 107, 255 },
    { 171, 63, 123, 255 },
    { 187, 83, 135, 255 },
    { 199, 103, 155, 255 },
    { 215, 127, 171, 255 },
    { 231, 155, 191, 255 },
    { 243, 195, 215, 255 },
    { 255, 235, 243, 255 },
    { 0, 0, 63, 255 },
    { 0, 0, 87, 255 },
    { 0, 0, 115, 255 },
    { 0, 0, 143, 255 },
    { 0, 0, 171, 255 },
    { 0, 0, 199, 255 },
    { 0, 7, 227, 255 },
    { 0, 7, 255, 255 },
    { 67, 79, 255, 255 },
    { 115, 123, 255, 255 },
    { 163, 171, 255, 255 },
    { 215, 219, 255, 255 },
    { 0, 39, 79, 255 },
    { 0, 51, 111, 255 },
    { 0, 63, 147, 255 },
    { 0, 71, 183, 255 },
    { 0, 79, 219, 255 },
    { 0, 83, 255, 255 },
    { 23, 111, 255, 255 },
    { 51, 139, 255, 255 },
    { 79, 163, 255, 255 },
    { 107, 183, 255, 255 },
    { 135, 203, 255, 255 },
    { 163, 219, 255, 255 },
    { 47, 51, 0, 255 },
    { 55, 63, 0, 255 },
    { 67, 75, 0, 255 },
    { 79, 87, 0, 255 },
    { 99, 107, 7, 255 },
    { 119, 127, 23, 255 },
    { 143, 147, 43, 255 },
    { 163, 167, 71, 255 },
    { 187, 187, 99, 255 },
    { 207, 207, 131, 255 },
    { 231, 231, 171, 255 },
    { 255, 255, 207, 255 },

    // 203 - 214 (Secondary remap)
    { 27, 0, 63, 255 },
    { 51, 0, 103, 255 },
    { 63, 11, 123, 255 },
    { 79, 23, 143, 255 },
    { 95, 31, 163, 255 },
    { 111, 39, 183, 255 },
    { 143, 59, 219, 255 },
    { 171, 91, 239, 255 },
    { 187, 119, 243, 255 },
    { 203, 151, 247, 255 },
    { 223, 183, 251, 255 },
    { 239, 215, 255, 255 },

    // 214 - 225 (Brown)
    { 0, 19, 39, 255 },
    { 7, 31, 55, 255 },
    { 15, 47, 71, 255 },
    { 31, 63, 91, 255 },
    { 51, 83, 107, 255 },
    { 75, 103, 123, 255 },
    { 107, 127, 143, 255 },
    { 127, 147, 163, 255 },
    { 147, 171, 187, 255 },
    { 171, 195, 207, 255 },
    { 195, 219, 231, 255 },
    { 223, 243, 255, 255 },

    // 226 (unknown)
    { 75, 75, 55, 255 },

    // 227 - 229 (tertiary remap)
    { 0, 183, 255, 255 },
    { 0, 219, 255, 255 },
    { 0, 255, 255, 255 },

    // 230 - 239 (water)
    { 99, 107, 7, 255 },
    { 99, 107, 7, 255 },
    { 135, 143, 39, 255 },
    { 123, 131, 27, 255 },
    { 99, 107, 7, 255 },
    { 151, 155, 55, 255 },
    { 151, 155, 55, 255 },
    { 227, 227, 155, 255 },
    { 203, 203, 115, 255 },
    { 151, 155, 55, 255 },

    // 240 - 242 (chain lift)
    { 91, 91, 67, 255 },
    { 107, 107, 83, 255 },
    { 123, 123, 99, 255 },

    // Old 243 - 245, changed to nice shade remap below
    // { 47, 47, 47, 255 },
    // { 47, 47, 47, 255 },
    // { 47, 71, 87, 255 },

    // 243 to 254 (primary remap)
    { 47, 51, 111, 255 },
    { 47, 55, 131, 255 },
    { 51, 63, 151, 255 },
    { 51, 67, 171, 255 },
    { 47, 75, 191, 255 },
    { 43, 79, 211, 255 },
    { 35, 87, 231, 255 },
    { 31, 95, 255, 255 },
    { 39, 127, 255, 255 },
    { 51, 155, 255, 255 },
    { 63, 183, 255, 255 },
    { 75, 207, 255, 255 },

    // 255 (unused?)
    { 0, 0, 0, 255 }
};
