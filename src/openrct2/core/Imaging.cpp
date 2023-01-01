/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4611) // interaction between '_setjmp' and C++ object destruction is non-portable

#include "Imaging.h"

#include "../Version.h"
#include "../drawing/Drawing.h"
#include "FileSystem.hpp"
#include "Guard.hpp"
#include "IStream.hpp"
#include "Memory.hpp"
#include "String.hpp"

#include <algorithm>
#include <fstream>
#include <png.h>
#include <stdexcept>
#include <unordered_map>

namespace Imaging
{
    constexpr auto EXCEPTION_IMAGE_FORMAT_UNKNOWN = "Unknown image format.";

    static std::unordered_map<IMAGE_FORMAT, ImageReaderFunc> _readerImplementations;

    static void PngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        auto istream = static_cast<std::istream*>(png_get_io_ptr(png_ptr));
        istream->read(reinterpret_cast<char*>(data), length);
    }

    static void PngWriteData(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        auto ostream = static_cast<std::ostream*>(png_get_io_ptr(png_ptr));
        ostream->write(reinterpret_cast<const char*>(data), length);
    }

    static void PngFlush(png_structp png_ptr)
    {
        auto ostream = static_cast<std::ostream*>(png_get_io_ptr(png_ptr));
        ostream->flush();
    }

    static void PngWarning(png_structp, const char* b)
    {
        log_warning(b);
    }

    static void PngError(png_structp, const char* b)
    {
        log_error(b);
    }

    static Image ReadPng(std::istream& istream, bool expandTo32)
    {
        png_structp png_ptr;
        png_infop info_ptr;

        try
        {
            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
            if (png_ptr == nullptr)
            {
                throw std::runtime_error("png_create_read_struct failed.");
            }

            info_ptr = png_create_info_struct(png_ptr);
            if (info_ptr == nullptr)
            {
                throw std::runtime_error("png_create_info_struct failed.");
            }

            // Set error handling
            if (setjmp(png_jmpbuf(png_ptr)))
            {
                throw std::runtime_error("png error.");
            }

            // Setup PNG reading
            int sig_read = 0;
            png_set_read_fn(png_ptr, &istream, PngReadData);
            png_set_sig_bytes(png_ptr, sig_read);

            uint32_t readFlags = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING;
            if (expandTo32)
            {
                // If we expand the resulting image always be full RGBA
                readFlags |= PNG_TRANSFORM_GRAY_TO_RGB | PNG_TRANSFORM_EXPAND;
            }
            png_read_png(png_ptr, info_ptr, readFlags, nullptr);

            // Read header
            png_uint_32 pngWidth, pngHeight;
            int bitDepth, colourType, interlaceType;
            png_get_IHDR(png_ptr, info_ptr, &pngWidth, &pngHeight, &bitDepth, &colourType, &interlaceType, nullptr, nullptr);

            // Read pixels as 32bpp RGBA data
            auto rowBytes = png_get_rowbytes(png_ptr, info_ptr);
            auto rowPointers = png_get_rows(png_ptr, info_ptr);
            auto pngPixels = std::vector<uint8_t>(pngWidth * pngHeight * 4);
            auto dst = pngPixels.data();
            if (colourType == PNG_COLOR_TYPE_RGB)
            {
                // 24-bit PNG (no alpha)
                Guard::Assert(rowBytes == pngWidth * 3, GUARD_LINE);
                for (png_uint_32 i = 0; i < pngHeight; i++)
                {
                    auto src = rowPointers[i];
                    for (png_uint_32 x = 0; x < pngWidth; x++)
                    {
                        *dst++ = *src++;
                        *dst++ = *src++;
                        *dst++ = *src++;
                        *dst++ = 255;
                    }
                }
            }
            else if (bitDepth == 8 && !expandTo32)
            {
                // 8-bit paletted or greyscale
                Guard::Assert(rowBytes == pngWidth, GUARD_LINE);
                for (png_uint_32 i = 0; i < pngHeight; i++)
                {
                    std::copy_n(rowPointers[i], rowBytes, dst);
                    dst += rowBytes;
                }
            }
            else
            {
                // 32-bit PNG (with alpha)
                Guard::Assert(rowBytes == pngWidth * 4, GUARD_LINE);
                for (png_uint_32 i = 0; i < pngHeight; i++)
                {
                    std::copy_n(rowPointers[i], rowBytes, dst);
                    dst += rowBytes;
                }
            }

            // Close the PNG
            png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

            // Return the output data
            Image img;
            img.Width = pngWidth;
            img.Height = pngHeight;
            img.Depth = expandTo32 ? 32 : 8;
            img.Pixels = std::move(pngPixels);
            img.Stride = pngWidth * (expandTo32 ? 4 : 1);
            return img;
        }
        catch (const std::exception&)
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
            throw;
        }
    }

    static void WritePng(std::ostream& ostream, const Image& image)
    {
        png_structp png_ptr = nullptr;
        png_colorp png_palette = nullptr;
        try
        {
            png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngError, PngWarning);
            if (png_ptr == nullptr)
            {
                throw std::runtime_error("png_create_write_struct failed.");
            }

            png_text text_ptr[1];
            text_ptr[0].key = const_cast<char*>("Software");
            text_ptr[0].text = const_cast<char*>(gVersionInfoFull);
            text_ptr[0].compression = PNG_TEXT_COMPRESSION_zTXt;

            auto info_ptr = png_create_info_struct(png_ptr);
            if (info_ptr == nullptr)
            {
                throw std::runtime_error("png_create_info_struct failed.");
            }

            if (image.Depth == 8)
            {
                if (image.Palette == nullptr)
                {
                    throw std::runtime_error("Expected a palette for 8-bit image.");
                }

                // Set the palette
                png_palette = static_cast<png_colorp>(png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(png_color)));
                if (png_palette == nullptr)
                {
                    throw std::runtime_error("png_malloc failed.");
                }
                for (size_t i = 0; i < PNG_MAX_PALETTE_LENGTH; i++)
                {
                    const auto& entry = (*image.Palette)[static_cast<uint16_t>(i)];
                    png_palette[i].blue = entry.Blue;
                    png_palette[i].green = entry.Green;
                    png_palette[i].red = entry.Red;
                }
                png_set_PLTE(png_ptr, info_ptr, png_palette, PNG_MAX_PALETTE_LENGTH);
            }

            png_set_write_fn(png_ptr, &ostream, PngWriteData, PngFlush);

            // Set error handler
            if (setjmp(png_jmpbuf(png_ptr)))
            {
                throw std::runtime_error("PNG ERROR");
            }

            // Write header
            auto colourType = PNG_COLOR_TYPE_RGB_ALPHA;
            if (image.Depth == 8)
            {
                png_byte transparentIndex = 0;
                png_set_tRNS(png_ptr, info_ptr, &transparentIndex, 1, nullptr);
                colourType = PNG_COLOR_TYPE_PALETTE;
            }
            png_set_text(png_ptr, info_ptr, text_ptr, 1);
            png_set_IHDR(
                png_ptr, info_ptr, image.Width, image.Height, 8, colourType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);
            png_write_info(png_ptr, info_ptr);

            // Write pixels
            auto pixels = image.Pixels.data();
            for (uint32_t y = 0; y < image.Height; y++)
            {
                png_write_row(png_ptr, const_cast<png_byte*>(pixels));
                pixels += image.Stride;
            }

            png_write_end(png_ptr, nullptr);
            png_destroy_info_struct(png_ptr, &info_ptr);
            png_free(png_ptr, png_palette);
            png_destroy_write_struct(&png_ptr, nullptr);
        }
        catch (const std::exception&)
        {
            png_free(png_ptr, png_palette);
            png_destroy_write_struct(&png_ptr, nullptr);
            throw;
        }
    }

    IMAGE_FORMAT GetImageFormatFromPath(std::string_view path)
    {
        if (String::EndsWith(path, ".png", true))
        {
            return IMAGE_FORMAT::PNG;
        }

        if (String::EndsWith(path, ".bmp", true))
        {
            return IMAGE_FORMAT::BITMAP;
        }

        return IMAGE_FORMAT::UNKNOWN;
    }

    static ImageReaderFunc GetReader(IMAGE_FORMAT format)
    {
        auto result = _readerImplementations.find(format);
        if (result != _readerImplementations.end())
        {
            return result->second;
        }
        return {};
    }

    void SetReader(IMAGE_FORMAT format, ImageReaderFunc impl)
    {
        _readerImplementations[format] = impl;
    }

    static Image ReadFromStream(std::istream& istream, IMAGE_FORMAT format)
    {
        switch (format)
        {
            case IMAGE_FORMAT::PNG:
                return ReadPng(istream, false);
            case IMAGE_FORMAT::PNG_32:
                return ReadPng(istream, true);
            case IMAGE_FORMAT::AUTOMATIC:
                throw std::invalid_argument("format can not be automatic.");
            default:
            {
                auto impl = GetReader(format);
                if (impl)
                {
                    return impl(istream, format);
                }
                throw std::runtime_error(EXCEPTION_IMAGE_FORMAT_UNKNOWN);
            }
        }
    }

    Image ReadFromFile(std::string_view path, IMAGE_FORMAT format)
    {
        switch (format)
        {
            case IMAGE_FORMAT::AUTOMATIC:
                return ReadFromFile(path, GetImageFormatFromPath(path));
            default:
            {
                std::ifstream fs(fs::u8path(path), std::ios::binary);
                return ReadFromStream(fs, format);
            }
        }
    }

    Image ReadFromBuffer(const std::vector<uint8_t>& buffer, IMAGE_FORMAT format)
    {
        ivstream<uint8_t> istream(buffer);
        return ReadFromStream(istream, format);
    }

    void WriteToFile(std::string_view path, const Image& image, IMAGE_FORMAT format)
    {
        switch (format)
        {
            case IMAGE_FORMAT::AUTOMATIC:
                WriteToFile(path, image, GetImageFormatFromPath(path));
                break;
            case IMAGE_FORMAT::PNG:
            {
                std::ofstream fs(fs::u8path(path), std::ios::binary);
                WritePng(fs, image);
                break;
            }
            default:
                throw std::runtime_error(EXCEPTION_IMAGE_FORMAT_UNKNOWN);
        }
    }
} // namespace Imaging
