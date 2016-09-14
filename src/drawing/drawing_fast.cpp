#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

extern "C"
{
    #include "drawing.h"
}

// This will have -1 (0xffffffff) for (val <= 0), 0 otherwise, so it can act as a mask
// This is expected to generate
//     sar eax, 0x1f (arithmetic shift right by 31)
#define less_or_equal_zero_mask(val) (((val - 1) >> (sizeof(val) * 8 - 1)))

template<int image_type, int zoom_level>
static void FASTCALL DrawRLESprite2(const uint8* RESTRICT source_bits_pointer,
                                      uint8* RESTRICT dest_bits_pointer,
                                      const uint8* RESTRICT palette_pointer,
                                      const rct_drawpixelinfo *RESTRICT dpi,
                                      int source_y_start,
                                      int height,
                                      int source_x_start,
                                      int width)
{
    int zoom_amount = 1 << zoom_level;
    int zoom_mask = 0xFFFFFFFF << zoom_level;
    uint8* next_dest_pointer = dest_bits_pointer;

    int line_width = (dpi->width >> zoom_level) + dpi->pitch;

    const int source_y_start_mask = less_or_equal_zero_mask(source_y_start + 1);
    source_y_start += zoom_amount & source_y_start_mask;
    next_dest_pointer += line_width & source_y_start_mask;
    height -= zoom_amount & source_y_start_mask;

    //For every line in the image
    for (int i = 0; i < height; i += zoom_amount) {
        int y = source_y_start + i;
        uint8 i2 = i >> zoom_level;

        //The first part of the source pointer is a list of offsets to different lines
        //This will move the pointer to the correct source line.
        const uint8 *next_source_pointer = source_bits_pointer + ((uint16*)source_bits_pointer)[y];
        uint8* loop_dest_pointer = next_dest_pointer + line_width * i2;

        uint8 last_data_line = 0;

        //For every data section in the line
        while (!last_data_line) {
            const uint8* source_pointer = next_source_pointer;
            uint8* dest_pointer = loop_dest_pointer;

            int no_pixels = *source_pointer++;
            //gap_size is the number of non drawn pixels you require to
            //jump over on your destination
            uint8 gap_size = *source_pointer++;
            //The last bit in no_pixels tells you if you have reached the end of a line
            last_data_line = no_pixels & 0x80;
            //Clear the last data line bit so we have just the no_pixels
            no_pixels &= 0x7f;
            //Have our next source pointer point to the next data section
            next_source_pointer = source_pointer + no_pixels;

            //Calculates the start point of the image
            int x_start = gap_size - source_x_start;
            const int x_diff = x_start & ~zoom_mask;
            const int x_mask = ~less_or_equal_zero_mask(x_diff);

            no_pixels -= x_diff;
            x_start += ~zoom_mask & x_mask;
            source_pointer += (x_start&~zoom_mask) & x_mask;

            // This will have -1 (0xffffffff) for (x_start <= 0), 0 otherwise
            int sign = less_or_equal_zero_mask(x_start);

            dest_pointer += (x_start >> zoom_level) & ~sign;

            //If the start is negative we require to remove part of the image.
            //This is done by moving the image pointer to the correct position.
            source_pointer -= x_start & sign;
            //The no_pixels will be reduced in this operation
            no_pixels += x_start & sign;
            //Reset the start position to zero as we have taken into account all moves
            x_start &= ~sign;

            int x_end = x_start + no_pixels;
            //If the end position is further out than the whole image
            //end position then we need to shorten the line again
            const int pixels_till_end = x_end - width;
            //Shorten the line
            no_pixels -= pixels_till_end & ~(less_or_equal_zero_mask(pixels_till_end));

            //Finally after all those checks, copy the image onto the drawing surface
            //If the image type is not a basic one we require to mix the pixels
            if (image_type & IMAGE_TYPE_REMAP) {//In the .exe these are all unraveled loops
                for (; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++) {
                    uint8 al = *source_pointer;
                    uint8 ah = *dest_pointer;
                    if (image_type & IMAGE_TYPE_TRANSPARENT)
                        al = palette_pointer[(((uint16)al << 8) | ah) - 0x100];
                    else
                        al = palette_pointer[al];
                    *dest_pointer = al;
                }
            } else if (image_type & IMAGE_TYPE_TRANSPARENT) {//In the .exe these are all unraveled loops
                //Doesn't use source pointer ??? mix with background only?
                //Not Tested

                for (; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++) {
                    uint8 pixel = *dest_pointer;
                    pixel = palette_pointer[pixel];
                    *dest_pointer = pixel;
                }
            } else
            {
                if (zoom_amount == 1) {
                    no_pixels &= ~less_or_equal_zero_mask(no_pixels);
                    memcpy(dest_pointer, source_pointer, no_pixels);
                } else {
                    for (; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++) {
                        *dest_pointer = *source_pointer;
                    }
                }
            }
        }
    }
}

#define DrawRLESpriteHelper2(image_type, zoom_level) \
    DrawRLESprite2<image_type, zoom_level>(source_bits_pointer, dest_bits_pointer, palette_pointer, dpi, source_y_start, height, source_x_start, width)

template<int image_type>
static void FASTCALL DrawRLESprite1(const uint8* source_bits_pointer,
                                      uint8* dest_bits_pointer,
                                      const uint8* palette_pointer,
                                      const rct_drawpixelinfo *dpi,
                                      int source_y_start,
                                      int height,
                                      int source_x_start,
                                      int width)
{
    int zoom_level = dpi->zoom_level;
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

extern "C"
{
    /**
     * Transfers readied images onto buffers
     * This function copies the sprite data onto the screen
     *  rct2: 0x0067AA18
     */
    void FASTCALL gfx_rle_sprite_to_buffer(const uint8* RESTRICT source_bits_pointer,
                                             uint8* RESTRICT dest_bits_pointer,
                                             const uint8* RESTRICT palette_pointer,
                                             const rct_drawpixelinfo * RESTRICT dpi,
                                             int image_type,
                                             int source_y_start,
                                             int height,
                                             int source_x_start,
                                             int width)
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
}
