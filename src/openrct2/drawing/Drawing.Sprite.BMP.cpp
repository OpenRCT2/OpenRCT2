/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

static void FASTCALL gfx_bmp_sprite_to_buffer_magnify(
    const PaletteMap& paletteMap, uint8_t* source_pointer, uint8_t* dest_pointer, const rct_g1_element* source_image,
    rct_drawpixelinfo* dest_dpi, int32_t height, int32_t width, ImageId imageId)
{
    auto zoom_level = dest_dpi->zoom_level;
    uint8_t zoom_amount = 1 * zoom_level;
    uint32_t dest_line_width = (dest_dpi->width / zoom_level) + dest_dpi->pitch;
    uint32_t source_line_width = source_image->width * zoom_level;

    // Basic bitmap with no draw pixels
    for (; height > 0; height -= zoom_amount)
    {
        auto next_source_pointer = source_pointer + source_line_width;
        auto next_dest_pointer = dest_pointer + dest_line_width;

        for (int32_t no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++, source_pointer += zoom_amount)
        {
            uint8_t pixel = *source_pointer;
            if (pixel)
            {
                *dest_pointer = pixel;
            }
        }
        dest_pointer = next_dest_pointer;
        source_pointer = next_source_pointer;
    }
}

/**
 * Copies a sprite onto the buffer. There is no compression used on the sprite
 * image.
 *  rct2: 0x0067A690
 * @param imageId Only flags are used.
 */
void FASTCALL gfx_bmp_sprite_to_buffer(
    const PaletteMap& paletteMap, uint8_t* source_pointer, uint8_t* dest_pointer, const rct_g1_element* source_image,
    rct_drawpixelinfo* dest_dpi, int32_t height, int32_t width, ImageId imageId)
{
    auto zoom_level = dest_dpi->zoom_level;
    if (zoom_level < 0)
    {
        gfx_bmp_sprite_to_buffer_magnify(
            paletteMap, source_pointer, dest_pointer, source_image, dest_dpi, height, width, imageId);
        return;
    }

    uint8_t zoom_amount = 1 * zoom_level;
    uint32_t dest_line_width = (dest_dpi->width / zoom_level) + dest_dpi->pitch;
    uint32_t source_line_width = source_image->width * zoom_level;

    // Image uses the palette pointer to remap the colours of the image
    if (imageId.HasPrimary())
    {
        // Image with remaps
        for (; height > 0; height -= zoom_amount)
        {
            uint8_t* next_source_pointer = source_pointer + source_line_width;
            uint8_t* next_dest_pointer = dest_pointer + dest_line_width;
            for (int32_t no_pixels = width; no_pixels > 0;
                 no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++)
            {
                uint8_t pixel = *source_pointer;
                pixel = paletteMap[pixel];
                if (pixel)
                {
                    *dest_pointer = pixel;
                }
            }

            source_pointer = next_source_pointer;
            dest_pointer = next_dest_pointer;
        }
        return;
    }

    // Image is transparent. It only uses source pointer for
    // telling if it needs to be drawn not for colour. Colour provided
    // by the palette pointer.
    if (imageId.IsBlended())
    { // Not tested
        for (; height > 0; height -= zoom_amount)
        {
            uint8_t* next_source_pointer = source_pointer + source_line_width;
            uint8_t* next_dest_pointer = dest_pointer + dest_line_width;

            for (int32_t no_pixels = width; no_pixels > 0;
                 no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++)
            {
                uint8_t pixel = *source_pointer;
                if (pixel)
                {
                    pixel = *dest_pointer;
                    pixel = paletteMap[pixel];
                    *dest_pointer = pixel;
                }
            }

            source_pointer = next_source_pointer;
            dest_pointer = next_dest_pointer;
        }
        return;
    }

    // Basic bitmap no fancy stuff
    if (!(source_image->flags & G1_FLAG_BMP))
    { // Not tested
        for (; height > 0; height -= zoom_amount)
        {
            uint8_t* next_source_pointer = source_pointer + source_line_width;
            uint8_t* next_dest_pointer = dest_pointer + dest_line_width;

            for (int32_t no_pixels = width; no_pixels > 0;
                 no_pixels -= zoom_amount, dest_pointer++, source_pointer += zoom_amount)
            {
                *dest_pointer = *source_pointer;
            }

            dest_pointer = next_dest_pointer;
            source_pointer = next_source_pointer;
        }
        return;
    }

    // Basic bitmap with no draw pixels
    for (; height > 0; height -= zoom_amount)
    {
        uint8_t* next_source_pointer = source_pointer + source_line_width;
        uint8_t* next_dest_pointer = dest_pointer + dest_line_width;

        for (int32_t no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++, source_pointer += zoom_amount)
        {
            uint8_t pixel = *source_pointer;
            if (pixel)
            {
                *dest_pointer = pixel;
            }
        }
        dest_pointer = next_dest_pointer;
        source_pointer = next_source_pointer;
    }
}
