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
#include <stdexcept>
#include <vector>
#include "../config/Config.h"
#include "../Context.h"
#include "../core/FileStream.hpp"
#include "../core/Path.hpp"
#include "../OpenRCT2.h"
#include "../platform/platform.h"
#include "../PlatformEnvironment.h"
#include "../sprites.h"
#include "../ui/UiContext.h"
#include "../util/Util.h"
#include "Drawing.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

#pragma pack(push, 1)
struct rct_g1_header
{
    uint32 num_entries;
    uint32 total_size;
};
assert_struct_size(rct_g1_header, 8);
#pragma pack(pop)

struct rct_gx
{
    rct_g1_header header;
    std::vector<rct_g1_element> elements;
    void * data;
};

constexpr struct
{
    int start;
    sint32 x_offset;
    sint32 y_offset;
}
sprite_peep_pickup_starts[15] =
{
    {SPR_PEEP_PICKUP_GUEST_START, 0, 15},
    {SPR_PEEP_PICKUP_HANDYMAN_START, 1, 18},
    {SPR_PEEP_PICKUP_MECHANIC_START, 3, 22},
    {SPR_PEEP_PICKUP_GUARD_START, 3, 15},
    {SPR_PEEP_PICKUP_PANDA_START, -1, 19},
    {SPR_PEEP_PICKUP_TIGER_START, -1, 17},
    {SPR_PEEP_PICKUP_ELEPHANT_START, -1, 17},
    {SPR_PEEP_PICKUP_GORILLA_START, 0, 17},
    {SPR_PEEP_PICKUP_SNOWMAN_START, -1, 16},
    {SPR_PEEP_PICKUP_KNIGHT_START, -2, 17},
    {SPR_PEEP_PICKUP_BANDIT_START, 0, 16},
    {SPR_PEEP_PICKUP_PIRATE_START, 0, 16},
    {SPR_PEEP_PICKUP_SHERIFF_START, 0, 16},
    {SPR_PEEP_PICKUP_ASTRONAUT_START, 0, 16},
    {SPR_PEEP_PICKUP_ROMAN_START, -1, 17},
};

static inline uint32 rctc_to_rct2_index(uint32 image)
{
    if      (                  image <  1542) return image;
    else if (image >=  1574 && image <  4983) return image - 32;
    else if (image >=  4986 && image < 17189) return image - 35;
    else if (image >= 17191 && image < 18121) return image - 37;
    else if (image >= 18123 && image < 23800) return image - 39;
    else if (image >= 23804 && image < 24670) return image - 43;
    else if (image >= 24674 && image < 28244) return image - 47;
    else if (image >= 28246                 ) return image - 49;
    else throw std::runtime_error("Invalid RCTC g1.dat file");
}

static void read_and_convert_gxdat(IStream * stream, size_t count, bool is_rctc, rct_g1_element *elements)
{
    auto g1Elements32 = std::make_unique<rct_g1_element_32bit[]>(count);
    stream->Read(g1Elements32.get(), count * sizeof(rct_g1_element_32bit));
    if (is_rctc)
    {
        // Process RCTC's g1.dat file
        uint32 rctc = 0;
        for (size_t i = 0; i < SPR_G1_END; ++i)
        {
            // RCTC's g1.dat has a number of additional elements
            // added between the RCT2 elements. This switch
            // statement skips over the elements we don't want.
            switch (i)
            {
            case 1542:
                rctc += 32; break;
            case 23761:
            case 24627:
                rctc += 4; break;
            case 4951:
                rctc += 3; break;
            case 17154:
            case 18084:
            case 28197:
                rctc += 2; break;
            }

            const rct_g1_element_32bit &src = g1Elements32[rctc];

            // Double cast to silence compiler warning about casting to
            // pointer from integer of mismatched length.
            elements[i].offset        = (uint8*)(uintptr_t)src.offset;
            elements[i].width         = src.width;
            elements[i].height        = src.height;
            elements[i].x_offset      = src.x_offset;
            elements[i].y_offset      = src.y_offset;
            elements[i].flags         = src.flags;

            if (src.flags & G1_FLAG_HAS_ZOOM_SPRITE)
            {
                elements[i].zoomed_offset = (uint16) (i - rctc_to_rct2_index(rctc - src.zoomed_offset));
            }
            else
            {
                elements[i].zoomed_offset = src.zoomed_offset;
            }

            ++rctc;
        }

        // The pincer graphic for picking up peeps is different in
        // RCTC, and the sprites have different offsets to accommodate
        // the change. This reverts the offsets to their RCT2 values.
        for (const auto &animation : sprite_peep_pickup_starts)
        {
            for (int i=0; i < SPR_PEEP_PICKUP_COUNT; ++i)
            {
                elements[animation.start + i].x_offset -= animation.x_offset;
                elements[animation.start + i].y_offset -= animation.y_offset;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < count; i++)
        {
            const rct_g1_element_32bit &src = g1Elements32[i];

            // Double cast to silence compiler warning about casting to
            // pointer from integer of mismatched length.
            elements[i].offset        = (uint8*)(uintptr_t)src.offset;
            elements[i].width         = src.width;
            elements[i].height        = src.height;
            elements[i].x_offset      = src.x_offset;
            elements[i].y_offset      = src.y_offset;
            elements[i].flags         = src.flags;
            elements[i].zoomed_offset = src.zoomed_offset;
        }
    }
}

void mask_scalar(sint32 width, sint32 height, const uint8 * RESTRICT maskSrc, const uint8 * RESTRICT colourSrc,
                 uint8 * RESTRICT dst, sint32 maskWrap, sint32 colourWrap, sint32 dstWrap)
{
    for (sint32 yy = 0; yy < height; yy++)
    {
        for (sint32 xx = 0; xx < width; xx++)
        {
            uint8 colour = (*colourSrc) & (*maskSrc);
            if (colour != 0)
            {
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

static std::string gfx_get_csg_header_path()
{
    auto path = Path::ResolveCasing(Path::Combine(gConfigGeneral.rct1_path, "Data", "csg1i.dat"));
    if (path.empty())
    {
        path = Path::ResolveCasing(Path::Combine(gConfigGeneral.rct1_path, "RCTdeluxe_install", "Data", "csg1i.dat"));
    }
    return path;
}

static std::string gfx_get_csg_data_path()
{
    // csg1.1 and csg1.dat are the same file.
    // In the CD version, it's called csg1.1 on the CD and csg1.dat on the disk.
    // In the GOG version, it's always called csg1.1.
    // In the Steam version, it's called csg1.dat in the "disk" folder and csg1.1 in the "CD" folder.
    auto path = Path::ResolveCasing(Path::Combine(gConfigGeneral.rct1_path, "Data", "csg1.1"));
    if (path.empty())
    {
        path = Path::ResolveCasing(Path::Combine(gConfigGeneral.rct1_path, "Data", "csg1.dat"));
    }
    return path;
}

static rct_gx   _g1 = { 0 };
static rct_gx   _g2 = { 0 };
static rct_gx   _csg = { 0 };
static bool     _csgLoaded = false;

static rct_g1_element _g1Temp = { nullptr };
bool gTinyFontAntiAliased = false;

/**
 *
 *  rct2: 0x00678998
 */
bool gfx_load_g1(void * platformEnvironment)
{
    auto env = (IPlatformEnvironment *)platformEnvironment;

    log_verbose("gfx_load_g1(...)");
    try
    {
        auto path = Path::Combine(env->GetDirectoryPath(DIRBASE::RCT2, DIRID::DATA), "g1.dat");
        auto fs = FileStream(path, FILE_MODE_OPEN);
        _g1.header = fs.ReadValue<rct_g1_header>();

        log_verbose("g1.dat, number of entries: %u", _g1.header.num_entries);

        if (_g1.header.num_entries < SPR_G1_END)
        {
            throw std::runtime_error("Not enough elements in g1.dat");
        }

        // Read element headers
        _g1.elements.resize(324206);
        bool is_rctc = _g1.header.num_entries == SPR_RCTC_G1_END;
        read_and_convert_gxdat(&fs, _g1.header.num_entries, is_rctc, _g1.elements.data());
        gTinyFontAntiAliased = is_rctc;

        // Read element data
        _g1.data = fs.ReadArray<uint8>(_g1.header.total_size);

        // Fix entry data offsets
        for (uint32 i = 0; i < _g1.header.num_entries; i++)
        {
            _g1.elements[i].offset += (uintptr_t)_g1.data;
        }
        return true;
    }
    catch (const std::exception &)
    {
        _g1.elements.clear();
        _g1.elements.shrink_to_fit();

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
    SafeFree(_g1.data);
    _g1.elements.clear();
    _g1.elements.shrink_to_fit();
}

void gfx_unload_g2()
{
    SafeFree(_g2.data);
    _g2.elements.clear();
    _g2.elements.shrink_to_fit();
}

void gfx_unload_csg()
{
    SafeFree(_csg.data);
    _csg.elements.clear();
    _csg.elements.shrink_to_fit();
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
        _g2.elements.resize(_g2.header.num_entries);
        read_and_convert_gxdat(&fs, _g2.header.num_entries, false, _g2.elements.data());

        // Read element data
        _g2.data = fs.ReadArray<uint8>(_g2.header.total_size);

        // Fix entry data offsets
        for (uint32 i = 0; i < _g2.header.num_entries; i++)
        {
            _g2.elements[i].offset += (uintptr_t)_g2.data;
        }
        return true;
    }
    catch (const std::exception &)
    {
        _g2.elements.clear();
        _g2.elements.shrink_to_fit();

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

    auto pathHeaderPath = gfx_get_csg_header_path();
    auto pathDataPath = gfx_get_csg_data_path();
    try
    {
        auto fileHeader = FileStream(pathHeaderPath, FILE_MODE_OPEN);
        auto fileData = FileStream(pathDataPath, FILE_MODE_OPEN);
        size_t fileHeaderSize = fileHeader.GetLength();
        size_t fileDataSize = fileData.GetLength();

        _csg.header.num_entries = (uint32)(fileHeaderSize / sizeof(rct_g1_element_32bit));
        _csg.header.total_size = (uint32)fileDataSize;

        if (_csg.header.num_entries < 69917)
        {
            log_warning("Cannot load CSG1.DAT, it has too few entries. Only CSG1.DAT from Loopy Landscapes will work.");
            return false;
        }

        // Read element headers
        _csg.elements.resize(_csg.header.num_entries);
        read_and_convert_gxdat(&fileHeader, _csg.header.num_entries, false, _csg.elements.data());

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
    catch (const std::exception &)
    {
        _csg.elements.clear();
        _csg.elements.shrink_to_fit();

        log_error("Unable to load csg graphics");
        return false;
    }
}

/**
 * Copies a sprite onto the buffer. There is no compression used on the sprite
 * image.
 *  rct2: 0x0067A690
 */
void FASTCALL gfx_bmp_sprite_to_buffer(const uint8* palette_pointer, uint8* unknown_pointer, uint8* source_pointer, uint8* dest_pointer, const rct_g1_element* source_image, rct_drawpixelinfo *dest_dpi, sint32 height, sint32 width, sint32 image_type)
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
        auto g1 = gfx_get_g1_element(palette_offset);
        if (g1 == nullptr)
        {
            return nullptr;
        }
        else
        {
            return g1->offset;
        }
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
            auto tertiary_palette = gfx_get_g1_element(tertiary_offset);
            if (tertiary_palette != nullptr)
            {
                memcpy(palette_pointer + 0x2E, &tertiary_palette->offset[0xF3], 12);
            }
        }
        auto primary_palette = gfx_get_g1_element(primary_offset);
        if (primary_palette != nullptr)
        {
            memcpy(palette_pointer + 0xF3, &primary_palette->offset[0xF3], 12);
        }
        auto secondary_palette = gfx_get_g1_element(secondary_offset);
        if (secondary_palette != nullptr)
        {
            memcpy(palette_pointer + 0xCA, &secondary_palette->offset[0xF3], 12);
        }

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

    const rct_g1_element * g1 = gfx_get_g1_element(image_element);
    if (g1 == nullptr)
    {
        return;
    }

    if (dpi->zoom_level != 0 && (g1->flags & G1_FLAG_HAS_ZOOM_SPRITE)) {
        rct_drawpixelinfo zoomed_dpi;
        zoomed_dpi.bits = dpi->bits;
        zoomed_dpi.x = dpi->x >> 1;
        zoomed_dpi.y = dpi->y >> 1;
        zoomed_dpi.height = dpi->height >> 1;
        zoomed_dpi.width = dpi->width >> 1;
        zoomed_dpi.pitch = dpi->pitch;
        zoomed_dpi.zoom_level = dpi->zoom_level - 1;
        gfx_draw_sprite_palette_set_software(&zoomed_dpi, image_type | (image_element - g1->zoomed_offset), x >> 1, y >> 1, palette_pointer, unknown_pointer);
        return;
    }

    if (dpi->zoom_level != 0 && (g1->flags & G1_FLAG_NO_ZOOM_DRAW)) {
        return;
    }

    // Its used super often so we will define it to a separate variable.
    sint32 zoom_level = dpi->zoom_level;
    sint32 zoom_mask = 0xFFFFFFFF << zoom_level;

    if (zoom_level && g1->flags & G1_FLAG_RLE_COMPRESSION){
        x -= ~zoom_mask;
        y -= ~zoom_mask;
    }

    // This will be the height of the drawn image
    sint32 height = g1->height;
    // This is the start y coordinate on the destination
    sint16 dest_start_y = y + g1->y_offset;

    // For whatever reason the RLE version does not use
    // the zoom mask on the y coordinate but does on x.
    if (g1->flags & G1_FLAG_RLE_COMPRESSION){
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
        if (g1->flags & G1_FLAG_RLE_COMPRESSION && zoom_level){
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
    sint32 width = g1->width;
    // This is the source start x coordinate
    sint32 source_start_x = 0;
    // This is the destination start x coordinate
    sint16 dest_start_x = ((x + g1->x_offset + ~zoom_mask)&zoom_mask) - dpi->x;

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
        if (g1->flags & G1_FLAG_RLE_COMPRESSION && zoom_level){
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

    uint8* dest_pointer = dpi->bits;
    // Move the pointer to the start point of the destination
    dest_pointer += ((dpi->width >> zoom_level) + dpi->pitch) * dest_start_y + dest_start_x;

    if (g1->flags & G1_FLAG_RLE_COMPRESSION){
        // We have to use a different method to move the source pointer for
        // rle encoded sprites so that will be handled within this function
        gfx_rle_sprite_to_buffer(g1->offset, dest_pointer, palette_pointer, dpi, image_type, source_start_y, height, source_start_x, width);
        return;
    }
    uint8* source_pointer = g1->offset;
    // Move the pointer to the start point of the source
    source_pointer += g1->width*source_start_y + source_start_x;

    if (!(g1->flags & G1_FLAG_1)) {
        gfx_bmp_sprite_to_buffer(palette_pointer, unknown_pointer, source_pointer, dest_pointer, g1, dpi, height, width, image_type);
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
    auto imgMask = gfx_get_g1_element(maskImage & 0x7FFFF);
    auto imgColour = gfx_get_g1_element(colourImage & 0x7FFFF);
    if (imgMask == nullptr || imgColour == nullptr)
    {
        return;
    }

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

    uint8 const * maskSrc   = imgMask->offset + (skipY * imgMask->width) + skipX;
    uint8 const * colourSrc = imgColour->offset + (skipY * imgColour->width) + skipX;
    uint8       * dst       = dpi->bits + (left - dpi->x) + ((top - dpi->y) * (dpi->width + dpi->pitch));

    sint32 maskWrap   = imgMask->width - width;
    sint32 colourWrap = imgColour->width - width;
    sint32 dstWrap    = ((dpi->width + dpi->pitch) - width);

    mask_fn(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
}

const rct_g1_element * gfx_get_g1_element(sint32 image_id)
{
    openrct2_assert(!gOpenRCT2NoGraphics, "gfx_get_g1_element called on headless instance");

    if (image_id == (-1 & 0x7FFFF))
    {
        return nullptr;
    }

    if (image_id == SPR_TEMP)
    {
        return &_g1Temp;
    }
    else if (image_id < SPR_G2_BEGIN)
    {
        if (image_id >= (sint32)_g1.elements.size())
        {
            return nullptr;
        }
        return &_g1.elements[image_id];
    }
    if (image_id < SPR_CSG_BEGIN)
    {
        const uint32 idx = image_id - SPR_G2_BEGIN;
        if (idx >= _g2.header.num_entries)
        {
            log_warning("Invalid entry in g2.dat requested, idx = %u. You may have to update your g2.dat.", idx);
            return nullptr;
        }
        return &_g2.elements[idx];
    }

    if (is_csg_loaded())
    {
        const uint32 idx = image_id - SPR_CSG_BEGIN;
        if (idx >= _csg.header.num_entries)
        {
            openrct2_assert(idx < _csg.header.num_entries,
                "Invalid entry in csg.dat requested, idx = %u.", idx);
            return nullptr;
        }
        return &_csg.elements[idx];
    }
    return nullptr;
}

void gfx_set_g1_element(sint32 imageId, const rct_g1_element * g1)
{
    openrct2_assert(!gOpenRCT2NoGraphics, "gfx_set_g1_element called on headless instance");
#ifdef DEBUG
    openrct2_assert((imageId >= 0 && imageId < SPR_G2_BEGIN) || imageId == SPR_TEMP, "gfx_set_g1_element called with unexpected image id");
    openrct2_assert(g1 != nullptr, "g1 was nullptr");
#endif

    if (imageId == SPR_TEMP)
    {
        _g1Temp = *g1;
    }
    else if (imageId >= 0 && imageId < SPR_G2_BEGIN)
    {
        if (imageId < (sint32)_g1.elements.size())
        {
            _g1.elements[imageId] = *g1;
        }
    }
}

bool is_csg_loaded()
{
    return _csgLoaded;
}

rct_size16 FASTCALL gfx_get_sprite_size(uint32 image_id)
{
    const rct_g1_element * g1 = gfx_get_g1_element(image_id & 0X7FFFF);
    rct_size16 size = {};
    if (g1 != nullptr)
    {
        size.width = g1->width;
        size.height = g1->height;
    }
    return size;
}

size_t g1_calculate_data_size(const rct_g1_element * g1)
{
    if (g1->flags & G1_FLAG_RLE_COMPRESSION)
    {
        uint16 * offsets = (uint16 *)g1->offset;
        uint8 * ptr = g1->offset + offsets[g1->height - 1];
        bool endOfLine = false;
        do
        {
            uint8 chunk0 = *ptr++;
            ptr++; // offset
            uint8 chunkSize = chunk0 & 0x7F;
            ptr += chunkSize;
            endOfLine = (chunk0 & 0x80) != 0;
        } while (!endOfLine);
        return ptr - g1->offset;
    }
    else
    {
        return g1->width * g1->height;
    }
}
