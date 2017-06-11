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

#include <memory>
#include "../common.h"
#include "../config/Config.h"
#include "../Context.h"
#include "../core/File.h"
#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Util.hpp"
#include "../OpenRCT2.h"
#include "../sprites.h"
#include "../ui/UiContext.h"

extern "C"
{
    #include "../rct2/addresses.h"
    #include "../util/util.h"
    #include "drawing.h"
}

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

extern "C"
{
    static void *   _g1Buffer = nullptr;
    static rct_gx   _g2 = { 0 };
    static rct_gx   _csg = { 0 };
    static bool     _csgLoaded = false;

    #ifdef NO_RCT2
        rct_g1_element * g1Elements = nullptr;
    #else
        rct_g1_element * g1Elements = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element);
    #endif

    static const uint32 FadeSprites[] =
    {
        (uint32)SPR_NONE,
        SPR_FADE_1,
        SPR_FADE_2,
        SPR_FADE_3,
        SPR_FADE_4,
        SPR_FADE_5,
        SPR_FADE_6,
        SPR_FADE_7,
    };

    static void read_and_convert_gxdat(IStream * stream, size_t count, rct_g1_element *elements)
    {
        auto g1Elements32 = stream->ReadArray<rct_g1_element_32bit>(count);
        for (size_t i = 0; i < count; i++)
        {
            const auto src = &g1Elements32[i];

            /* Double cast to silence compiler warning about casting to
             * pointer from integer of mismatched length.
             */
            elements[i].offset        = (uint8*)(uintptr_t)src->offset;
            elements[i].width         = src->width;
            elements[i].height        = src->height;
            elements[i].x_offset      = src->x_offset;
            elements[i].y_offset      = src->y_offset;
            elements[i].flags         = src->flags;
            elements[i].zoomed_offset = src->zoomed_offset;
        }
        Memory::Free(g1Elements32);
    }

    /**
     *
     *  rct2: 0x00678998
     */
    bool gfx_load_g1()
    {
        log_verbose("gfx_load_g1()");
        try
        {
            auto fs = FileStream(get_file_path(PATH_ID_G1), FILE_MODE_OPEN);
            rct_g1_header header = fs.ReadValue<rct_g1_header>();

            /* number of elements is stored in g1.dat, but because the entry
             * headers are static, this can't be variable until made into a
             * dynamic array.
             */
            header.num_entries = 29294;

            // Read element headers
#ifdef NO_RCT2
            g1Elements = Memory::AllocateArray<rct_g1_element>(324206);
#endif
            read_and_convert_gxdat(&fs, header.num_entries, g1Elements);

            // Read element data
            _g1Buffer = fs.ReadArray<uint8>(header.total_size);

            // Fix entry data offsets
            for (uint32 i = 0; i < header.num_entries; i++)
            {
                g1Elements[i].offset += (uintptr_t)_g1Buffer;
            }
            return true;
        }
        catch (const Exception &)
        {
            log_fatal("Unable to load g1 graphics");
            if (!gOpenRCT2Headless)
            {
                IUiContext * uiContext = GetContext()->GetUiContext();
                uiContext->ShowMessageBox("Unable to load g1.dat. Your RollerCoaster Tycoon 2 path may be incorrectly set.");
            }
            return false;
        }
    }

    void gfx_unload_g1()
    {
        SafeFree(_g1Buffer);
    #ifdef NO_RCT2
        SafeFree(g1Elements);
    #endif
    }

    void gfx_unload_g2()
    {
        SafeFree(_g2.elements);
        SafeFree(_g2.data);
    }

    void gfx_unload_csg()
    {
        SafeFree(_csg.elements);
        SafeFree(_csg.data);
    }

    bool gfx_load_g2()
    {
        log_verbose("gfx_load_g2()");

        char path[MAX_PATH];

        platform_get_openrct_data_path(path, sizeof(path));
        safe_strcat_path(path, "g2.dat", MAX_PATH);
        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            _g2.header = fs.ReadValue<rct_g1_header>();

            // Read element headers
            _g2.elements = Memory::AllocateArray<rct_g1_element>(_g2.header.num_entries);
            read_and_convert_gxdat(&fs, _g2.header.num_entries, _g2.elements);

            // Read element data
            _g2.data = fs.ReadArray<uint8>(_g2.header.total_size);

            // Fix entry data offsets
            for (uint32 i = 0; i < _g2.header.num_entries; i++)
            {
                _g2.elements[i].offset += (uintptr_t)_g2.data;
            }
            return true;
        }
        catch (const Exception &)
        {
            log_fatal("Unable to load g2 graphics");
            if (!gOpenRCT2Headless)
            {
                IUiContext * uiContext = GetContext()->GetUiContext();
                uiContext->ShowMessageBox("Unable to load g2.dat");
            }
        }
        return false;
    }

    bool gfx_load_csg()
    {
        log_verbose("gfx_load_csg()");

        if (str_is_null_or_empty(gConfigGeneral.rct1_path))
        {
            log_verbose("  unable to load CSG, RCT1 path not set");
            return false;
        }

        char pathHeader[MAX_PATH];
        safe_strcpy(pathHeader, gConfigGeneral.rct1_path, sizeof(pathHeader));
        safe_strcat_path(pathHeader, "Data", sizeof(pathHeader));
        safe_strcat_path(pathHeader, "csg1i.dat", sizeof(pathHeader));

        // csg1.1 and csg1.dat are the same file.
        // In the CD version, it's called csg1.1 on the CD and csg1.dat on the disk.
        // In the GOG version, it's always called csg1.1.
        // In the Steam version, it's always called csg1.dat.
        char pathData[MAX_PATH];
        safe_strcpy(pathData, gConfigGeneral.rct1_path, sizeof(pathData));
        safe_strcat_path(pathData, "Data", sizeof(pathData));
        safe_strcat_path(pathData, "csg1.1", sizeof(pathData));

        if (!File::Exists(pathData))
        {
            safe_strcpy(pathData, gConfigGeneral.rct1_path, sizeof(pathData));
            safe_strcat_path(pathData, "Data", sizeof(pathData));
            safe_strcat_path(pathData, "csg1.dat", sizeof(pathData));
        }

        try
        {
            auto fileHeader = FileStream(pathHeader, FILE_MODE_OPEN);
            auto fileData = FileStream(pathData, FILE_MODE_OPEN);
            size_t fileHeaderSize = fileHeader.GetLength();
            size_t fileDataSize = fileData.GetLength();

            _csg.header.num_entries = (uint32)(fileHeaderSize / sizeof(rct_g1_element_32bit));
            _csg.header.total_size = (uint32)fileDataSize;

            // Read element headers
            _csg.elements = Memory::AllocateArray<rct_g1_element>(_csg.header.num_entries);
            read_and_convert_gxdat(&fileHeader, _csg.header.num_entries, _csg.elements);

            // Read element data
            _csg.data = fileData.ReadArray<uint8>(_csg.header.total_size);

            // Fix entry data offsets
            for (uint32 i = 0; i < _csg.header.num_entries; i++)
            {
                _csg.elements[i].offset += (uintptr_t)_csg.data;
                // RCT1 used zoomed offsets that counted from the beginning of the file, rather than from the current sprite.
                _csg.elements[i].zoomed_offset = i - (SPR_CSG_BEGIN + _csg.elements[i].zoomed_offset);
            }
            _csgLoaded = true;
            return true;
        }
        catch (const Exception &)
        {
            log_error("Unable to load csg graphics");
            return false;
        }
    }

    /**
     * This function looks like it initialises the 0x009E3CE4 array which references sprites used for background / palette mixing or
     * something. Further investigation is needed.
     */
    void sub_68371D()
    {
        for (size_t i = 0; i < Util::CountOf(FadeSprites); i++)
        {
            const uint32 spriteId = FadeSprites[i];
            if (spriteId == (uint32)SPR_NONE)
            {
                unk_9E3CE4[i] = nullptr;
            }
            else
            {
                unk_9E3CE4[i] = g1Elements[FadeSprites[i]].offset;
            }
        }
    }

    /**
     * Copies a sprite onto the buffer. There is no compression used on the sprite
     * image.
     *  rct2: 0x0067A690
     */
    void FASTCALL gfx_bmp_sprite_to_buffer(uint8* palette_pointer, uint8* unknown_pointer, uint8* source_pointer, uint8* dest_pointer, rct_g1_element* source_image, rct_drawpixelinfo *dest_dpi, sint32 height, sint32 width, sint32 image_type)
    {
        uint16 zoom_level = dest_dpi->zoom_level;
        uint8 zoom_amount = 1 << zoom_level;
        uint32 dest_line_width = (dest_dpi->width / zoom_amount) + dest_dpi->pitch;
        uint32 source_line_width = source_image->width * zoom_amount;

        // Image uses the palette pointer to remap the colours of the image
        if (image_type & IMAGE_TYPE_REMAP){
            assert(palette_pointer != nullptr);

            // Image with remaps
            for (; height > 0; height -= zoom_amount){
                uint8* next_source_pointer = source_pointer + source_line_width;
                uint8* next_dest_pointer = dest_pointer + dest_line_width;
                for (sint32 no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++){
                    uint8 pixel = *source_pointer;
                    pixel = palette_pointer[pixel];
                    if (pixel){
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
        if (image_type & IMAGE_TYPE_TRANSPARENT){ // Not tested
            assert(palette_pointer != nullptr);
            for (; height > 0; height -= zoom_amount){
                uint8* next_source_pointer = source_pointer + source_line_width;
                uint8* next_dest_pointer = dest_pointer + dest_line_width;

                for (sint32 no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, source_pointer += zoom_amount, dest_pointer++){
                    uint8 pixel = *source_pointer;
                    if (pixel){
                        pixel = *dest_pointer;
                        pixel = palette_pointer[pixel];
                        *dest_pointer = pixel;
                    }
                }

                source_pointer = next_source_pointer;
                dest_pointer = next_dest_pointer;
            }
            return;
        }

        // Basic bitmap no fancy stuff
        if (!(source_image->flags & G1_FLAG_BMP)){ // Not tested
            for (; height > 0; height -= zoom_amount){
                uint8* next_source_pointer = source_pointer + source_line_width;
                uint8* next_dest_pointer = dest_pointer + dest_line_width;

                for (sint32 no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++, source_pointer += zoom_amount){
                    *dest_pointer = *source_pointer;
                }

                dest_pointer = next_dest_pointer;
                source_pointer = next_source_pointer;
            }
            return;
        }

        // Basic bitmap with no draw pixels
        for (; height > 0; height -= zoom_amount){
            uint8* next_source_pointer = source_pointer + source_line_width;
            uint8* next_dest_pointer = dest_pointer + dest_line_width;

            for (sint32 no_pixels = width; no_pixels > 0; no_pixels -= zoom_amount, dest_pointer++, source_pointer += zoom_amount){
                uint8 pixel = *source_pointer;
                if (pixel){
                    *dest_pointer = pixel;
                }
            }
            dest_pointer = next_dest_pointer;
            source_pointer = next_source_pointer;
        }
        return;
    }

    uint8* FASTCALL gfx_draw_sprite_get_palette(sint32 image_id, uint32 tertiary_colour) {
        sint32 image_type = (image_id & 0xE0000000);
        if (image_type == 0)
            return nullptr;

        if (!(image_type & IMAGE_TYPE_REMAP_2_PLUS)) {
            uint8 palette_ref = (image_id >> 19) & 0xFF;
            if (!(image_type & IMAGE_TYPE_TRANSPARENT)) {
                palette_ref &= 0x7F;
            }

            uint16 palette_offset = palette_to_g1_offset[palette_ref];
            return g1Elements[palette_offset].offset;
        }
        else {
            uint8* palette_pointer = gPeepPalette;

            uint32 primary_offset = palette_to_g1_offset[(image_id >> 19) & 0x1F];
            uint32 secondary_offset = palette_to_g1_offset[(image_id >> 24) & 0x1F];

            if (!(image_type & IMAGE_TYPE_REMAP)) {
                palette_pointer = gOtherPalette;
    #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                assert(tertiary_colour < PALETTE_TO_G1_OFFSET_COUNT);
    #endif // DEBUG_LEVEL_2
                uint32 tertiary_offset = palette_to_g1_offset[tertiary_colour];
                rct_g1_element* tertiary_palette = &g1Elements[tertiary_offset];
                memcpy(palette_pointer + 0x2E, &tertiary_palette->offset[0xF3], 12);
            }
            rct_g1_element* primary_palette = &g1Elements[primary_offset];
            rct_g1_element* secondary_palette = &g1Elements[secondary_offset];

            memcpy(palette_pointer + 0xF3, &primary_palette->offset[0xF3], 12);
            memcpy(palette_pointer + 0xCA, &secondary_palette->offset[0xF3], 12);

            return palette_pointer;
        }
    }

    /**
     *
     *  rct2: 0x0067A28E
     * image_id (ebx)
     * image_id as below
     * 0b_111X_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX image_type
     * 0b_XXX1_11XX_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX image_sub_type (unknown pointer)
     * 0b_XXX1_1111_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX secondary_colour
     * 0b_XXXX_XXXX_1111_1XXX_XXXX_XXXX_XXXX_XXXX primary_colour
     * 0b_XXXX_X111_1111_1XXX_XXXX_XXXX_XXXX_XXXX palette_ref
     * 0b_XXXX_XXXX_XXXX_X111_1111_1111_1111_1111 image_id (offset to g1)
     * x (cx)
     * y (dx)
     * dpi (esi)
     * tertiary_colour (ebp)
     */
    void FASTCALL gfx_draw_sprite_software(rct_drawpixelinfo *dpi, sint32 image_id, sint32 x, sint32 y, uint32 tertiary_colour)
    {
        if (image_id != -1)
        {
            uint8* palette_pointer = gfx_draw_sprite_get_palette(image_id, tertiary_colour);
            if (image_id & IMAGE_TYPE_REMAP_2_PLUS) {
                image_id |= IMAGE_TYPE_REMAP;
            }

            gfx_draw_sprite_palette_set_software(dpi, image_id, x, y, palette_pointer, nullptr);
        }
    }

    /*
    * rct: 0x0067A46E
    * image_id (ebx) and also (0x00EDF81C)
    * palette_pointer (0x9ABDA4)
    * unknown_pointer (0x9E3CDC)
    * dpi (edi)
    * x (cx)
    * y (dx)
    */
    void FASTCALL gfx_draw_sprite_palette_set_software(rct_drawpixelinfo *dpi, sint32 image_id, sint32 x, sint32 y, uint8* palette_pointer, uint8* unknown_pointer)
    {
        sint32 image_element = image_id & 0x7FFFF;
        sint32 image_type = image_id & 0xE0000000;

        rct_g1_element *g1_source = gfx_get_g1_element(image_element);
        if (g1_source == nullptr)
        {
            return;
        }

        if (dpi->zoom_level != 0 && (g1_source->flags & G1_FLAG_HAS_ZOOM_SPRITE)) {
            rct_drawpixelinfo zoomed_dpi;
            zoomed_dpi.bits = dpi->bits;
            zoomed_dpi.x = dpi->x >> 1;
            zoomed_dpi.y = dpi->y >> 1;
            zoomed_dpi.height = dpi->height >> 1;
            zoomed_dpi.width = dpi->width >> 1;
            zoomed_dpi.pitch = dpi->pitch;
            zoomed_dpi.zoom_level = dpi->zoom_level - 1;
            gfx_draw_sprite_palette_set_software(&zoomed_dpi, image_type | (image_element - g1_source->zoomed_offset), x >> 1, y >> 1, palette_pointer, unknown_pointer);
            return;
        }

        if (dpi->zoom_level != 0 && (g1_source->flags & G1_FLAG_NO_ZOOM_DRAW)) {
            return;
        }

        // Its used super often so we will define it to a separate variable.
        sint32 zoom_level = dpi->zoom_level;
        sint32 zoom_mask = 0xFFFFFFFF << zoom_level;

        if (zoom_level && g1_source->flags & G1_FLAG_RLE_COMPRESSION){
            x -= ~zoom_mask;
            y -= ~zoom_mask;
        }

        // This will be the height of the drawn image
        sint32 height = g1_source->height;
        // This is the start y coordinate on the destination
        sint16 dest_start_y = y + g1_source->y_offset;

        // For whatever reason the RLE version does not use
        // the zoom mask on the y coordinate but does on x.
        if (g1_source->flags & G1_FLAG_RLE_COMPRESSION){
            dest_start_y -= dpi->y;
        }
        else{
            dest_start_y = (dest_start_y&zoom_mask) - dpi->y;
        }
        //This is the start y coordinate on the source
        sint32 source_start_y = 0;

        if (dest_start_y < 0){
            // If the destination y is negative reduce the height of the
            // image as we will cut off the bottom
            height += dest_start_y;
            // If the image is no longer visible nothing to draw
            if (height <= 0){
                return;
            }
            // The source image will start a further up the image
            source_start_y -= dest_start_y;
            // The destination start is now reset to 0
            dest_start_y = 0;
        }
        else{
            if (g1_source->flags & G1_FLAG_RLE_COMPRESSION && zoom_level){
                source_start_y -= dest_start_y & ~zoom_mask;
                height += dest_start_y & ~zoom_mask;
            }
        }

        sint32 dest_end_y = dest_start_y + height;

        if (dest_end_y > dpi->height){
            // If the destination y is outside of the drawing
            // image reduce the height of the image
            height -= dest_end_y - dpi->height;
        }
        // If the image no longer has anything to draw
        if (height <= 0)return;

        dest_start_y >>= zoom_level;

        // This will be the width of the drawn image
        sint32 width = g1_source->width;
        // This is the source start x coordinate
        sint32 source_start_x = 0;
        // This is the destination start x coordinate
        sint16 dest_start_x = ((x + g1_source->x_offset + ~zoom_mask)&zoom_mask) - dpi->x;

        if (dest_start_x < 0){
            // If the destination is negative reduce the width
            // image will cut off the side
            width += dest_start_x;
            // If there is no image to draw
            if (width <= 0){
                return;
            }
            // The source start will also need to cut off the side
            source_start_x -= dest_start_x;
            // Reset the destination to 0
            dest_start_x = 0;
        }
        else{
            if (g1_source->flags & G1_FLAG_RLE_COMPRESSION && zoom_level){
                source_start_x -= dest_start_x & ~zoom_mask;
            }
        }

        sint32 dest_end_x = dest_start_x + width;

        if (dest_end_x > dpi->width){
            // If the destination x is outside of the drawing area
            // reduce the image width.
            width -= dest_end_x - dpi->width;
            // If there is no image to draw.
            if (width <= 0)return;
        }

        dest_start_x >>= zoom_level;

        uint8* dest_pointer = (uint8*)dpi->bits;
        // Move the pointer to the start point of the destination
        dest_pointer += ((dpi->width >> zoom_level) + dpi->pitch) * dest_start_y + dest_start_x;

        if (g1_source->flags & G1_FLAG_RLE_COMPRESSION){
            // We have to use a different method to move the source pointer for
            // rle encoded sprites so that will be handled within this function
            gfx_rle_sprite_to_buffer(g1_source->offset, dest_pointer, palette_pointer, dpi, image_type, source_start_y, height, source_start_x, width);
            return;
        }
        uint8* source_pointer = g1_source->offset;
        // Move the pointer to the start point of the source
        source_pointer += g1_source->width*source_start_y + source_start_x;

        if (!(g1_source->flags & G1_FLAG_1)) {
            gfx_bmp_sprite_to_buffer(palette_pointer, unknown_pointer, source_pointer, dest_pointer, g1_source, dpi, height, width, image_type);
        }
    }

    /**
     * Draws the given colour image masked out by the given mask image. This can currently only cope with bitmap formatted mask and
     * colour images. Presumably the original game never used RLE images for masking. Colour 0 represents transparent.
     *
     *  rct2: 0x00681DE2
     */
    void FASTCALL gfx_draw_sprite_raw_masked_software(rct_drawpixelinfo *dpi, sint32 x, sint32 y, sint32 maskImage, sint32 colourImage)
    {
        sint32 left, top, right, bottom, width, height;
        rct_g1_element *imgMask = &g1Elements[maskImage & 0x7FFFF];
        rct_g1_element *imgColour = &g1Elements[colourImage & 0x7FFFF];

        assert(imgMask->flags & G1_FLAG_BMP);
        assert(imgColour->flags & G1_FLAG_BMP);

        if (dpi->zoom_level != 0) {
            // TODO: Implement other zoom levels (probably not used though)
            assert(false);
            return;
        }

        width = Math::Min(imgMask->width, imgColour->width);
        height = Math::Min(imgMask->height, imgColour->height);

        x += imgMask->x_offset;
        y += imgMask->y_offset;

        left = Math::Max<sint32>(dpi->x, x);
        top = Math::Max<sint32>(dpi->y, y);
        right = Math::Min(dpi->x + dpi->width, x + width);
        bottom = Math::Min(dpi->y + dpi->height, y + height);

        width = right - left;
        height = bottom - top;
        if (width < 0 || height < 0)
            return;

        sint32 skipX = left - x;
        sint32 skipY = top - y;

        uint8 *maskSrc = imgMask->offset + (skipY * imgMask->width) + skipX;
        uint8 *colourSrc = imgColour->offset + (skipY * imgColour->width) + skipX;
        uint8 *dst = dpi->bits + (left - dpi->x) + ((top - dpi->y) * (dpi->width + dpi->pitch));

        sint32 maskWrap = imgMask->width - width;
        sint32 colourWrap = imgColour->width - width;
        sint32 dstWrap = ((dpi->width + dpi->pitch) - width);
        for (sint32 yy = top; yy < bottom; yy++) {
            for (sint32 xx = left; xx < right; xx++) {
                uint8 colour = (*colourSrc) & (*maskSrc);
                if (colour != 0) {
                    *dst = colour;
                }

                maskSrc++;
                colourSrc++;
                dst++;
            }
            maskSrc += maskWrap;
            colourSrc += colourWrap;
            dst += dstWrap;
        }
    }

    rct_g1_element * gfx_get_g1_element(sint32 image_id)
    {
        if (image_id == (-1 & 0x7FFFF))
        {
            return nullptr;
        }

        if (image_id < SPR_G2_BEGIN)
        {
            return &g1Elements[image_id];
        }
        if (image_id < SPR_CSG_BEGIN)
        {
            const uint32 idx = image_id - SPR_G2_BEGIN;
            openrct2_assert(idx < _g2.header.num_entries,
                            "Invalid entry in g2.dat requested, idx = %u. You may have to update your g2.dat.", idx);
            return &_g2.elements[idx];
        }

        if (is_csg_loaded())
        {
            const uint32 idx = image_id - SPR_CSG_BEGIN;
            openrct2_assert(idx < _csg.header.num_entries,
                            "Invalid entry in csg.dat requested, idx = %u.", idx);
            return &_csg.elements[idx];
        }
        return nullptr;
    }

    bool is_csg_loaded()
    {
        return _csgLoaded;
    }
}
