/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4127) // conditional expression is constant

#include <cstring>

#include "Drawing.h"

template<int32_t image_type, int32_t zoom_level>
static void FASTCALL DrawRLESprite2(const uint8_t* RESTRICT source_bits_pointer,
                                      uint8_t* RESTRICT dest_bits_pointer,
                                      const uint8_t* RESTRICT palette_pointer,
                                      const rct_drawpixelinfo *RESTRICT dpi,
                                      int32_t source_y_start,
                                      int32_t height,
                                      int32_t source_x_start,
                                      int32_t width)
{
    // The distance between two samples in the source image.
    // We draw the image at 1 / (2^zoom_level) scale.
    int32_t zoom_amount = 1 << zoom_level;

    // Width of one screen line in the dest buffer
    int32_t line_width = (dpi->width >> zoom_level) + dpi->pitch;

    // Move up to the first line of the image if source_y_start is negative. Why does this even occur?
    if (source_y_start < 0)
    {
        source_y_start    += zoom_amount;
        height            -= zoom_amount;
        dest_bits_pointer += line_width;
    }

    //For every line in the image
    for (int32_t i = 0; i < height; i += zoom_amount)
    {
        int32_t y = source_y_start + i;

        //The first part of the source pointer is a list of offsets to different lines
        //This will move the pointer to the correct source line.
        const uint8_t *lineData = source_bits_pointer + ((uint16_t*)source_bits_pointer)[y];
        uint8_t* loop_dest_pointer = dest_bits_pointer + line_width * (i >> zoom_level);

        uint8_t isEndOfLine = 0;

        // For every data chunk in the line
        while (!isEndOfLine)
        {
            const uint8_t* copySrc = lineData;
            //uint8_t* copyDest = loop_dest_pointer;

            // Read chunk metadata
            uint8_t dataSize    = *copySrc++;
            uint8_t firstPixelX = *copySrc++;

            isEndOfLine = dataSize & 0x80;  // If the last bit in dataSize is set, then this is the last line
            dataSize &= 0x7F;               // The rest of the bits are the actual size

            //Have our next source pointer point to the next data section
            lineData = copySrc + dataSize;

            int32_t x_start = firstPixelX - source_x_start;
            int32_t numPixels = dataSize;

            if (x_start > 0)
            {
                int mod = x_start & (zoom_amount - 1);  // x_start modulo zoom_amount

                // If x_start is not a multiple of zoom_amount, round it up to a multiple
                if (mod != 0)
                {
                    int offset = zoom_amount - mod;
                    x_start   += offset;
                    copySrc   += offset;
                    numPixels -= offset;
                }
            }
            else if (x_start < 0)
            {
                // Clamp x_start to zero if negative
                int offset = 0 - x_start;
                x_start = 0;
                copySrc   += offset;
                numPixels -= offset;
            }

            //If the end position is further out than the whole image
            //end position then we need to shorten the line again
            if (x_start + numPixels > width)
                numPixels = width - x_start;

            uint8_t *copyDest = loop_dest_pointer + (x_start >> zoom_level);

            //Finally after all those checks, copy the image onto the drawing surface
            //If the image type is not a basic one we require to mix the pixels
            if (image_type & IMAGE_TYPE_REMAP)  // palette controlled images
            {
                for (int j = 0; j < numPixels; j += zoom_amount, copySrc += zoom_amount, copyDest++)
                {
                    if (image_type & IMAGE_TYPE_TRANSPARENT)
                    {
                        uint16_t color = ((*copySrc << 8) | *copyDest) - 0x100;
                        *copyDest = palette_pointer[color];
                    }
                    else
                    {
                        *copyDest = palette_pointer[*copySrc];
                    }
                }
            }
            else if (image_type & IMAGE_TYPE_TRANSPARENT)  // single alpha blended color (used for glass)
            {
                for (int j = 0; j < numPixels; j += zoom_amount, copyDest++)
                {
                    uint8_t pixel = *copyDest;
                    pixel = palette_pointer[pixel];
                    *copyDest = pixel;
                }
            }
            else  // standard opaque image
            {
                if (zoom_level == 0)
                {
                    // Since we're sampling each pixel at this zoom level, just do a straight memcpy
                    if (numPixels > 0)
                        memcpy(copyDest, copySrc, numPixels);
                }
                else
                {
                    for (int j = 0; j < numPixels; j += zoom_amount, copySrc += zoom_amount, copyDest++)
                        *copyDest = *copySrc;
                }
            }
        }
    }
}

#define DrawRLESpriteHelper2(image_type, zoom_level) \
    DrawRLESprite2<image_type, zoom_level>(source_bits_pointer, dest_bits_pointer, palette_pointer, dpi, source_y_start, height, source_x_start, width)

template<int32_t image_type>
static void FASTCALL DrawRLESprite1(const uint8_t* source_bits_pointer,
                                      uint8_t* dest_bits_pointer,
                                      const uint8_t* palette_pointer,
                                      const rct_drawpixelinfo *dpi,
                                      int32_t source_y_start,
                                      int32_t height,
                                      int32_t source_x_start,
                                      int32_t width)
{
    int32_t zoom_level = dpi->zoom_level;
    switch (zoom_level) {
    case 0: DrawRLESpriteHelper2(image_type, 0); break;
    case 1: DrawRLESpriteHelper2(image_type, 1); break;
    case 2: DrawRLESpriteHelper2(image_type, 2); break;
    case 3: DrawRLESpriteHelper2(image_type, 3); break;
    default: assert(false); break;
    }
}

#define DrawRLESpriteHelper1(image_type) \
    DrawRLESprite1<image_type>(source_bits_pointer, dest_bits_pointer, palette_pointer, dpi, source_y_start, height, source_x_start, width)

/**
 * Transfers readied images onto buffers
 * This function copies the sprite data onto the screen
 *  rct2: 0x0067AA18
 */
void FASTCALL gfx_rle_sprite_to_buffer(const uint8_t* RESTRICT source_bits_pointer,
                                         uint8_t* RESTRICT dest_bits_pointer,
                                         const uint8_t* RESTRICT palette_pointer,
                                         const rct_drawpixelinfo * RESTRICT dpi,
                                         int32_t image_type,
                                         int32_t source_y_start,
                                         int32_t height,
                                         int32_t source_x_start,
                                         int32_t width)
{
    if (image_type & IMAGE_TYPE_REMAP)
    {
        if (image_type & IMAGE_TYPE_TRANSPARENT)
        {
            DrawRLESpriteHelper1(IMAGE_TYPE_REMAP | IMAGE_TYPE_TRANSPARENT);
        }
        else
        {
            DrawRLESpriteHelper1(IMAGE_TYPE_REMAP);
        }
    }
    else if (image_type & IMAGE_TYPE_TRANSPARENT)
    {
        DrawRLESpriteHelper1(IMAGE_TYPE_TRANSPARENT);
    }
    else
    {
        DrawRLESpriteHelper1(IMAGE_TYPE_DEFAULT);
    }
}

