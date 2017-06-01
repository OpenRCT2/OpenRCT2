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

#pragma warning(disable : 4611) // interaction between '_setjmp' and C++ object destruction is non-portable

#include <png.h>
#include "core/Exception.hpp"
#include "core/FileStream.hpp"
#include "core/Guard.hpp"
#include "core/Memory.hpp"

#include "Imaging.h"

namespace Imaging
{
    static void PngReadData(png_structp png_ptr, png_bytep data, png_size_t length);
    static void PngWriteData(png_structp png_ptr, png_bytep data, png_size_t length);
    static void PngFlush(png_structp png_ptr);
    static void PngWarning(png_structp png_ptr, const char * b);
    static void PngError(png_structp png_ptr, const char * b);

    bool PngRead(uint8 * * pixels, uint32 * width, uint32 * height, const utf8 * path)
    {
        png_structp png_ptr;
        png_infop info_ptr;

        // Setup PNG structures
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (png_ptr == nullptr)
        {
            return false;
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == nullptr)
        {
            png_destroy_read_struct(&png_ptr, nullptr, nullptr);
            return false;
        }

        // Open PNG file
        try
        {
            unsigned int sig_read = 0;
            auto fs = FileStream(path, FILE_MODE_OPEN);

            // Set error handling
            if (setjmp(png_jmpbuf(png_ptr)))
            {
                png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
                return false;
            }

            // Setup PNG reading
            png_set_read_fn(png_ptr, &fs, PngReadData);
            png_set_sig_bytes(png_ptr, sig_read);

            // To simplify the reading process, convert 4-16 bit data to 24-32 bit data
            png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_GRAY_TO_RGB, nullptr);

            // Read header
            png_uint_32 pngWidth, pngHeight;
            int bitDepth, colourType, interlaceType;
            png_get_IHDR(png_ptr, info_ptr, &pngWidth, &pngHeight, &bitDepth, &colourType, &interlaceType, nullptr, nullptr);

            // Read pixels as 32bpp RGBA data
            png_size_t rowBytes = png_get_rowbytes(png_ptr, info_ptr);
            png_bytepp rowPointers = png_get_rows(png_ptr, info_ptr);
            uint8 * pngPixels = Memory::Allocate<uint8>(pngWidth * pngHeight * 4);
            uint8 * dst = pngPixels;
            if (colourType == PNG_COLOR_TYPE_RGB)
            {
                // 24-bit PNG (no alpha)
                Guard::Assert(rowBytes == pngWidth * 3, GUARD_LINE);
                for (png_uint_32 i = 0; i < pngHeight; i++)
                {
                    uint8 * src = rowPointers[i];
                    for (png_uint_32 x = 0; x < pngWidth; x++)
                    {
                        *dst++ = *src++;
                        *dst++ = *src++;
                        *dst++ = *src++;
                        *dst++ = 255;
                    }
                }
            }
            else
            {
                // 32-bit PNG (with alpha)
                Guard::Assert(rowBytes == pngWidth * 4, GUARD_LINE);
                for (png_uint_32 i = 0; i < pngHeight; i++)
                {
                    Memory::Copy(dst, rowPointers[i], rowBytes);
                    dst += rowBytes;
                }
            }

            // Close the PNG
            png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

            // Return the output data
            *pixels = (uint8*)pngPixels;
            if (width != nullptr) *width = pngWidth;
            if (height != nullptr) *height = pngHeight;

            return true;
        }
        catch (Exception)
        {
            *pixels = nullptr;
            if (width != nullptr) *width = 0;
            if (height != nullptr) *height = 0;
            return false;
        }
    }

    bool PngWrite(const rct_drawpixelinfo * dpi, const rct_palette * palette, const utf8 * path)
    {
        bool result = false;

        // Get image size
        int stride = dpi->width + dpi->pitch;

        // Setup PNG
        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (png_ptr == nullptr)
        {
            return false;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == nullptr)
        {
            png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);
            return false;
        }

        png_colorp png_palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));
        for (int i = 0; i < 256; i++)
        {
            const rct_palette_entry *entry = &palette->entries[i];
            png_palette[i].blue = entry->blue;
            png_palette[i].green = entry->green;
            png_palette[i].red = entry->red;
        }

        png_set_PLTE(png_ptr, info_ptr, png_palette, PNG_MAX_PALETTE_LENGTH);

        try
        {
            // Open file for writing
            auto fs = FileStream(path, FILE_MODE_WRITE);
            png_set_write_fn(png_ptr, &fs, PngWriteData, PngFlush);

            // Set error handler
            if (setjmp(png_jmpbuf(png_ptr)))
            {
                throw Exception("PNG ERROR");
            }

            // Write header
            png_set_IHDR(
                png_ptr, info_ptr, dpi->width, dpi->height, 8,
                PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
            );
            png_byte transparentIndex = 0;
            png_set_tRNS(png_ptr, info_ptr, &transparentIndex, 1, nullptr);
            png_write_info(png_ptr, info_ptr);

            // Write pixels
            uint8 * bits = dpi->bits;
            for (int y = 0; y < dpi->height; y++)
            {
                png_write_row(png_ptr, (png_byte *)bits);
                bits += stride;
            }

            // Finish
            png_write_end(png_ptr, nullptr);
            result = true;
        }
        catch (Exception)
        {
        }

        png_free(png_ptr, png_palette);
        png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);
        return result;
    }

    bool PngWrite32bpp(sint32 width, sint32 height, const void * pixels, const utf8 * path)
    {
        bool result = false;

        // Setup PNG
        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngError, PngWarning);
        if (png_ptr == nullptr)
        {
            return false;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == nullptr)
        {
            png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);
            return false;
        }

        try
        {
            // Open file for writing
            auto fs = FileStream(path, FILE_MODE_WRITE);
            png_set_write_fn(png_ptr, &fs, PngWriteData, PngFlush);

            // Set error handler
            if (setjmp(png_jmpbuf(png_ptr)))
            {
                throw Exception("PNG ERROR");
            }

            // Write header
            png_set_IHDR(
                png_ptr, info_ptr, width, height, 8,
                PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
            );
            png_write_info(png_ptr, info_ptr);

            // Write pixels
            uint8 * bits = (uint8 *)pixels;
            for (int y = 0; y < height; y++)
            {
                png_write_row(png_ptr, (png_byte *)bits);
                bits += width * 4;
            }

            // Finish
            png_write_end(png_ptr, nullptr);
            result = true;
        }
        catch (Exception)
        {
        }

        png_destroy_write_struct(&png_ptr, &info_ptr);
        return result;
    }

    static void PngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        auto * fs = static_cast<FileStream *>(png_get_io_ptr(png_ptr));
        fs->Read(data, length);
    }

    static void PngWriteData(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        auto * fs = static_cast<FileStream *>(png_get_io_ptr(png_ptr));
        fs->Write(data, length);
    }

    static void PngFlush(png_structp png_ptr)
    {
    }

    static void PngWarning(png_structp png_ptr, const char * b)
    {
        log_warning(b);
    }

    static void PngError(png_structp png_ptr, const char * b)
    {
        log_error(b);
    }
}

extern "C"
{
    bool image_io_png_read(uint8 * * pixels, uint32 * width, uint32 * height, const utf8 * path)
    {
        return Imaging::PngRead(pixels, width, height, path);
    }

    bool image_io_png_write(const rct_drawpixelinfo * dpi, const rct_palette * palette, const utf8 * path)
    {
        return Imaging::PngWrite(dpi, palette, path);
    }

    bool image_io_png_write_32bpp(sint32 width, sint32 height, const void * pixels, const utf8 * path)
    {
        return Imaging::PngWrite32bpp(width, height, pixels, path);
    }
}
