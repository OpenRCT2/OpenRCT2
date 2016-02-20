extern "C"
{
    #include "drawing.h"
}

template<int image_type, int zoom_level>
static void FASTCALL DrawRLESprite2(const uint8* source_bits_pointer,
                                      uint8* dest_bits_pointer,
                                      const uint8* palette_pointer,
                                      const rct_drawpixelinfo *dpi,
                                      int source_y_start,
                                      int height,
                                      int source_x_start,
                                      int width)
{
    int zoom_amount = 1 << zoom_level;
    int zoom_mask = 0xFFFFFFFF << zoom_level;
    uint8* next_dest_pointer = dest_bits_pointer;

    int line_width = (dpi->width >> zoom_level) + dpi->pitch;

    if (source_y_start < 0) {
        source_y_start += zoom_amount;
        next_dest_pointer += line_width;
        height -= zoom_amount;
    }

    //For every line in the image
    for (int y = source_y_start; y < (height + source_y_start); y += zoom_amount) {

        //The first part of the source pointer is a list of offsets to different lines
        //This will move the pointer to the correct source line.
        const uint8 *next_source_pointer = source_bits_pointer + ((uint16*)source_bits_pointer)[y];

        uint8 last_data_line = 0;

        //For every data section in the line
        while (!last_data_line) {
            const uint8* source_pointer = next_source_pointer;
            uint8* dest_pointer = next_dest_pointer;

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

            if (x_start & ~zoom_mask) {
                no_pixels -= (x_start&~zoom_mask);
                x_start += ~zoom_mask;
                source_pointer += (x_start&~zoom_mask);
                if (no_pixels <= 0) continue;
            }

            if (x_start > 0) {
                //Since the start is positive
                //We need to move the drawing surface to the correct position
                dest_pointer += x_start >> zoom_level;
            } else {
                //If the start is negative we require to remove part of the image.
                //This is done by moving the image pointer to the correct position.
                source_pointer -= x_start;
                //The no_pixels will be reduced in this operation
                no_pixels += x_start;
                //If there are no pixels there is nothing to draw this data section
                if (no_pixels <= 0) continue;
                //Reset the start position to zero as we have taken into account all moves
                x_start = 0;
            }

            int x_end = x_start + no_pixels;
            //If the end position is further out than the whole image
            //end position then we need to shorten the line again
            if (x_end > width) {
                //Shorten the line
                no_pixels -= x_end - width;
                //If there are no pixels there is nothing to draw.
                if (no_pixels <= 0) continue;
            }

            //Finally after all those checks, copy the image onto the drawing surface
            //If the image type is not a basic one we require to mix the pixels
            if (image_type & IMAGE_TYPE_USE_PALETTE) {//In the .exe these are all unraveled loops
                for (; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++) {
                    uint8 al = *source_pointer;
                    uint8 ah = *dest_pointer;
                    if (image_type & IMAGE_TYPE_MIX_BACKGROUND)
                        al = palette_pointer[(((uint16)al << 8) | ah) - 0x100];
                    else
                        al = palette_pointer[al];
                    *dest_pointer = al;
                }
            } else if (image_type & IMAGE_TYPE_MIX_BACKGROUND) {//In the .exe these are all unraveled loops
                //Doesnt use source pointer ??? mix with background only?
                //Not Tested

                for (; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++) {
                    uint8 pixel = *dest_pointer;
                    pixel = palette_pointer[pixel];
                    *dest_pointer = pixel;
                }
            } else
            {
                if (zoom_amount == 1) {
                    memcpy(dest_pointer, source_pointer, no_pixels);
                } else {
                    for (; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++) {
                        *dest_pointer = *source_pointer;
                    }
                }
            }
        }

        //Add a line to the drawing surface pointer
        next_dest_pointer += line_width;
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
    void FASTCALL gfx_rle_sprite_to_buffer(const uint8* source_bits_pointer,
                                             uint8* dest_bits_pointer,
                                             const uint8* palette_pointer,
                                             const rct_drawpixelinfo *dpi,
                                             int image_type,
                                             int source_y_start,
                                             int height,
                                             int source_x_start,
                                             int width)
    {
        if (image_type & IMAGE_TYPE_USE_PALETTE)
        {
            if (image_type & IMAGE_TYPE_MIX_BACKGROUND)
            {
                DrawRLESpriteHelper1(IMAGE_TYPE_USE_PALETTE | IMAGE_TYPE_MIX_BACKGROUND);
            }
            else
            {
                DrawRLESpriteHelper1(IMAGE_TYPE_USE_PALETTE);
            }
        }
        else if (image_type & IMAGE_TYPE_MIX_BACKGROUND)
        {
            DrawRLESpriteHelper1(IMAGE_TYPE_MIX_BACKGROUND);
        }
        else
        {
            DrawRLESpriteHelper1(0);
        }
    }
}
