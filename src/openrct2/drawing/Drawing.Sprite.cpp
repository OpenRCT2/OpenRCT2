/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../config/Config.h"
#include "../core/FileStream.h"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../platform/Platform.h"
#include "../sprites.h"
#include "../ui/UiContext.h"
#include "../util/Util.h"
#include "ScrollingText.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

// clang-format off
constexpr struct
{
    int start;
    int32_t x_offset;
    int32_t y_offset;
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

static inline uint32_t rctc_to_rct2_index(uint32_t image)
{
    if (                  image <  1542) return image;
    if (image >=  1574 && image <  4983) return image - 32;
    if (image >=  4986 && image < 17189) return image - 35;
    if (image >= 17191 && image < 18121) return image - 37;
    if (image >= 18123 && image < 23800) return image - 39;
    if (image >= 23804 && image < 24670) return image - 43;
    if (image >= 24674 && image < 28244) return image - 47;
    if (image >= 28246                 ) return image - 49;
    throw std::runtime_error("Invalid RCTC g1.dat file");
}
// clang-format on

static void ReadAndConvertGxDat(IStream* stream, size_t count, bool is_rctc, G1Element* elements)
{
    auto g1Elements32 = std::make_unique<RCTG1Element[]>(count);
    stream->Read(g1Elements32.get(), count * sizeof(RCTG1Element));
    if (is_rctc)
    {
        // Process RCTC's g1.dat file
        uint32_t rctc = 0;
        for (size_t i = 0; i < SPR_G1_END; ++i)
        {
            // RCTC's g1.dat has a number of additional elements
            // added between the RCT2 elements. This switch
            // statement skips over the elements we don't want.
            switch (i)
            {
                case 1542:
                    rctc += 32;
                    break;
                case 23761:
                case 24627:
                    rctc += 4;
                    break;
                case 4951:
                    rctc += 3;
                    break;
                case 17154:
                case 18084:
                case 28197:
                    rctc += 2;
                    break;
            }

            const RCTG1Element& src = g1Elements32[rctc];

            // Double cast to silence compiler warning about casting to
            // pointer from integer of mismatched length.
            elements[i].offset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(src.offset));
            elements[i].width = src.width;
            elements[i].height = src.height;
            elements[i].x_offset = src.x_offset;
            elements[i].y_offset = src.y_offset;
            elements[i].flags = src.flags;

            if (src.flags & G1_FLAG_HAS_ZOOM_SPRITE)
            {
                elements[i].zoomed_offset = static_cast<int32_t>(i - rctc_to_rct2_index(rctc - src.zoomed_offset));
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
        for (const auto& animation : sprite_peep_pickup_starts)
        {
            for (int i = 0; i < SPR_PEEP_PICKUP_COUNT; ++i)
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
            const RCTG1Element& src = g1Elements32[i];

            // Double cast to silence compiler warning about casting to
            // pointer from integer of mismatched length.
            elements[i].offset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(src.offset));
            elements[i].width = src.width;
            elements[i].height = src.height;
            elements[i].x_offset = src.x_offset;
            elements[i].y_offset = src.y_offset;
            elements[i].flags = src.flags;
            elements[i].zoomed_offset = src.zoomed_offset;
        }
    }
}

void MaskScalar(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    for (int32_t yy = 0; yy < height; yy++)
    {
        for (int32_t xx = 0; xx < width; xx++)
        {
            uint8_t colour = (*colourSrc) & (*maskSrc);
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

static Gx _g1 = {};
static Gx _g2 = {};
static Gx _csg = {};
static G1Element _scrollingText[MaxScrollingTextEntries]{};
static bool _csgLoaded = false;

static G1Element _g1Temp = {};
static std::vector<G1Element> _imageListElements;
bool gTinyFontAntiAliased = false;

/**
 *
 *  rct2: 0x00678998
 */
bool GfxLoadG1(const IPlatformEnvironment& env)
{
    LOG_VERBOSE("GfxLoadG1(...)");
    try
    {
        auto path = env.FindFile(DIRBASE::RCT2, DIRID::DATA, u8"g1.dat");
        auto fs = FileStream(path, FILE_MODE_OPEN);
        _g1.header = fs.ReadValue<RCTG1Header>();

        LOG_VERBOSE("g1.dat, number of entries: %u", _g1.header.num_entries);

        if (_g1.header.num_entries < SPR_G1_END)
        {
            throw std::runtime_error("Not enough elements in g1.dat");
        }

        // Read element headers
        bool is_rctc = _g1.header.num_entries == SPR_RCTC_G1_END;
        _g1.elements.resize(_g1.header.num_entries);
        ReadAndConvertGxDat(&fs, _g1.header.num_entries, is_rctc, _g1.elements.data());
        gTinyFontAntiAliased = is_rctc;

        // Read element data
        _g1.data = fs.ReadArray<uint8_t>(_g1.header.total_size);

        // Fix entry data offsets
        for (uint32_t i = 0; i < _g1.header.num_entries; i++)
        {
            _g1.elements[i].offset += reinterpret_cast<uintptr_t>(_g1.data.get());
        }
        return true;
    }
    catch (const std::exception&)
    {
        _g1.elements.clear();
        _g1.elements.shrink_to_fit();

        LOG_FATAL("Unable to load g1 graphics");
        if (!gOpenRCT2Headless)
        {
            auto uiContext = GetContext()->GetUiContext();
            uiContext->ShowMessageBox("Unable to load g1.dat. Your RollerCoaster Tycoon 2 path may be incorrectly set.");
        }
        return false;
    }
}

void GfxUnloadG1()
{
    _g1.data.reset();
    _g1.elements.clear();
    _g1.elements.shrink_to_fit();
}

void GfxUnloadG2()
{
    _g2.data.reset();
    _g2.elements.clear();
    _g2.elements.shrink_to_fit();
}

void GfxUnloadCsg()
{
    _csg.data.reset();
    _csg.elements.clear();
    _csg.elements.shrink_to_fit();
}

bool GfxLoadG2()
{
    LOG_VERBOSE("GfxLoadG2()");

    auto env = GetContext()->GetPlatformEnvironment();

    std::string path = Path::Combine(env->GetDirectoryPath(DIRBASE::OPENRCT2), u8"g2.dat");

    try
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        _g2.header = fs.ReadValue<RCTG1Header>();

        // Read element headers
        _g2.elements.resize(_g2.header.num_entries);
        ReadAndConvertGxDat(&fs, _g2.header.num_entries, false, _g2.elements.data());

        // Read element data
        _g2.data = fs.ReadArray<uint8_t>(_g2.header.total_size);

        if (_g2.header.num_entries != G2_SPRITE_COUNT)
        {
            std::string errorMessage = "Mismatched g2.dat size.\nExpected: " + std::to_string(G2_SPRITE_COUNT) + "\nActual: "
                + std::to_string(_g2.header.num_entries) + "\ng2.dat may be installed improperly.\nPath to g2.dat: " + path;

            LOG_ERROR(errorMessage.c_str());

            if (!gOpenRCT2Headless)
            {
                auto uiContext = GetContext()->GetUiContext();
                uiContext->ShowMessageBox(errorMessage);
                uiContext->ShowMessageBox("Warning: You may experience graphical glitches if you continue. It's recommended "
                                          "that you update g2.dat if you're seeing this message");
            }
        }

        // Fix entry data offsets
        for (uint32_t i = 0; i < _g2.header.num_entries; i++)
        {
            _g2.elements[i].offset += reinterpret_cast<uintptr_t>(_g2.data.get());
        }
        return true;
    }
    catch (const std::exception&)
    {
        _g2.elements.clear();
        _g2.elements.shrink_to_fit();

        LOG_FATAL("Unable to load g2 graphics");
        if (!gOpenRCT2Headless)
        {
            auto uiContext = GetContext()->GetUiContext();
            uiContext->ShowMessageBox("Unable to load g2.dat");
        }
    }
    return false;
}

bool GfxLoadCsg()
{
    LOG_VERBOSE("GfxLoadCsg()");

    if (gConfigGeneral.RCT1Path.empty())
    {
        LOG_VERBOSE("  unable to load CSG, RCT1 path not set");
        return false;
    }

    auto pathHeaderPath = FindCsg1idatAtLocation(gConfigGeneral.RCT1Path);
    auto pathDataPath = FindCsg1datAtLocation(gConfigGeneral.RCT1Path);
    try
    {
        auto fileHeader = FileStream(pathHeaderPath, FILE_MODE_OPEN);
        auto fileData = FileStream(pathDataPath, FILE_MODE_OPEN);
        size_t fileHeaderSize = fileHeader.GetLength();
        size_t fileDataSize = fileData.GetLength();

        _csg.header.num_entries = static_cast<uint32_t>(fileHeaderSize / sizeof(RCTG1Element));
        _csg.header.total_size = static_cast<uint32_t>(fileDataSize);

        if (!CsgIsUsable(_csg))
        {
            LOG_WARNING("Cannot load CSG1.DAT, it has too few entries. Only CSG1.DAT from Loopy Landscapes will work.");
            return false;
        }

        // Read element headers
        _csg.elements.resize(_csg.header.num_entries);
        ReadAndConvertGxDat(&fileHeader, _csg.header.num_entries, false, _csg.elements.data());

        // Read element data
        _csg.data = fileData.ReadArray<uint8_t>(_csg.header.total_size);

        // Fix entry data offsets
        for (uint32_t i = 0; i < _csg.header.num_entries; i++)
        {
            _csg.elements[i].offset += reinterpret_cast<uintptr_t>(_csg.data.get());
            // RCT1 used zoomed offsets that counted from the beginning of the file, rather than from the current sprite.
            if (_csg.elements[i].flags & G1_FLAG_HAS_ZOOM_SPRITE)
            {
                _csg.elements[i].zoomed_offset = i - _csg.elements[i].zoomed_offset;
            }
        }
        _csgLoaded = true;
        return true;
    }
    catch (const std::exception&)
    {
        _csg.elements.clear();
        _csg.elements.shrink_to_fit();

        LOG_ERROR("Unable to load csg graphics");
        return false;
    }
}

std::optional<Gx> GfxLoadGx(const std::vector<uint8_t>& buffer)
{
    try
    {
        OpenRCT2::MemoryStream istream(buffer.data(), buffer.size());
        Gx gx;

        gx.header = istream.ReadValue<RCTG1Header>();

        // Read element headers
        gx.elements.resize(gx.header.num_entries);
        ReadAndConvertGxDat(&istream, gx.header.num_entries, false, gx.elements.data());

        // Read element data
        gx.data = istream.ReadArray<uint8_t>(gx.header.total_size);

        return std::make_optional(std::move(gx));
    }
    catch (const std::exception&)
    {
        LOG_VERBOSE("Unable to load Gx graphics");
    }
    return std::nullopt;
}

static std::optional<PaletteMap> FASTCALL GfxDrawSpriteGetPalette(ImageId imageId)
{
    if (!imageId.HasSecondary())
    {
        uint8_t paletteId = imageId.GetRemap();
        if (!imageId.IsBlended())
        {
            paletteId &= 0x7F;
        }
        return GetPaletteMapForColour(paletteId);
    }

    auto paletteMap = PaletteMap(gPeepPalette);
    if (imageId.HasTertiary())
    {
        paletteMap = PaletteMap(gOtherPalette);
        auto tertiaryPaletteMap = GetPaletteMapForColour(imageId.GetTertiary());
        if (tertiaryPaletteMap.has_value())
        {
            paletteMap.Copy(
                PALETTE_OFFSET_REMAP_TERTIARY, tertiaryPaletteMap.value(), PALETTE_OFFSET_REMAP_PRIMARY, PALETTE_LENGTH_REMAP);
        }
    }

    auto primaryPaletteMap = GetPaletteMapForColour(imageId.GetPrimary());
    if (primaryPaletteMap.has_value())
    {
        paletteMap.Copy(
            PALETTE_OFFSET_REMAP_PRIMARY, primaryPaletteMap.value(), PALETTE_OFFSET_REMAP_PRIMARY, PALETTE_LENGTH_REMAP);
    }

    auto secondaryPaletteMap = GetPaletteMapForColour(imageId.GetSecondary());
    if (secondaryPaletteMap.has_value())
    {
        paletteMap.Copy(
            PALETTE_OFFSET_REMAP_SECONDARY, secondaryPaletteMap.value(), PALETTE_OFFSET_REMAP_PRIMARY, PALETTE_LENGTH_REMAP);
    }

    return paletteMap;
}

void FASTCALL GfxDrawSpriteSoftware(DrawPixelInfo* dpi, const ImageId imageId, const ScreenCoordsXY& spriteCoords)
{
    if (imageId.HasValue())
    {
        auto palette = GfxDrawSpriteGetPalette(imageId);
        if (!palette)
        {
            palette = PaletteMap::GetDefault();
        }
        GfxDrawSpritePaletteSetSoftware(dpi, imageId, spriteCoords, *palette);
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
void FASTCALL GfxDrawSpritePaletteSetSoftware(
    DrawPixelInfo* dpi, const ImageId imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap)
{
    int32_t x = coords.x;
    int32_t y = coords.y;

    const auto* g1 = GfxGetG1Element(imageId);
    if (g1 == nullptr)
    {
        return;
    }

    if (dpi->zoom_level > ZoomLevel{ 0 } && (g1->flags & G1_FLAG_HAS_ZOOM_SPRITE))
    {
        DrawPixelInfo zoomed_dpi = *dpi;
        zoomed_dpi.bits = dpi->bits;
        zoomed_dpi.x = dpi->x >> 1;
        zoomed_dpi.y = dpi->y >> 1;
        zoomed_dpi.height = dpi->height >> 1;
        zoomed_dpi.width = dpi->width >> 1;
        zoomed_dpi.pitch = dpi->pitch;
        zoomed_dpi.zoom_level = dpi->zoom_level - 1;

        const auto spriteCoords = ScreenCoordsXY{ x >> 1, y >> 1 };
        GfxDrawSpritePaletteSetSoftware(
            &zoomed_dpi, imageId.WithIndex(imageId.GetIndex() - g1->zoomed_offset), spriteCoords, paletteMap);
        return;
    }

    if (dpi->zoom_level > ZoomLevel{ 0 } && (g1->flags & G1_FLAG_NO_ZOOM_DRAW))
    {
        return;
    }

    // Its used super often so we will define it to a separate variable.
    const auto zoom_level = dpi->zoom_level;
    const int32_t zoom_mask = zoom_level > ZoomLevel{ 0 } ? zoom_level.ApplyTo(0xFFFFFFFF) : 0xFFFFFFFF;

    if (zoom_level > ZoomLevel{ 0 } && g1->flags & G1_FLAG_RLE_COMPRESSION)
    {
        x -= ~zoom_mask;
        y -= ~zoom_mask;
    }

    // This will be the height of the drawn image
    int32_t height = g1->height;

    // This is the start y coordinate on the destination
    int16_t dest_start_y = y + g1->y_offset;

    // For whatever reason the RLE version does not use
    // the zoom mask on the y coordinate but does on x.
    if (g1->flags & G1_FLAG_RLE_COMPRESSION)
    {
        dest_start_y -= dpi->y;
    }
    else
    {
        dest_start_y = (dest_start_y & zoom_mask) - dpi->y;
    }
    // This is the start y coordinate on the source
    int32_t source_start_y = 0;

    if (dest_start_y < 0)
    {
        // If the destination y is negative reduce the height of the
        // image as we will cut off the bottom
        height += dest_start_y;
        // If the image is no longer visible nothing to draw
        if (height <= 0)
        {
            return;
        }
        // The source image will start a further up the image
        source_start_y -= dest_start_y;
        // The destination start is now reset to 0
        dest_start_y = 0;
    }
    else
    {
        if ((g1->flags & G1_FLAG_RLE_COMPRESSION) && zoom_level > ZoomLevel{ 0 })
        {
            source_start_y -= dest_start_y & ~zoom_mask;
            height += dest_start_y & ~zoom_mask;
        }
    }

    int32_t dest_end_y = dest_start_y + height;

    if (dest_end_y > dpi->height)
    {
        // If the destination y is outside of the drawing
        // image reduce the height of the image
        height -= dest_end_y - dpi->height;
    }
    // If the image no longer has anything to draw
    if (height <= 0)
        return;

    dest_start_y = zoom_level.ApplyInversedTo(dest_start_y);

    // This will be the width of the drawn image
    int32_t width = g1->width;

    // This is the source start x coordinate
    int32_t source_start_x = 0;
    // This is the destination start x coordinate
    int16_t dest_start_x = ((x + g1->x_offset + ~zoom_mask) & zoom_mask) - dpi->x;

    if (dest_start_x < 0)
    {
        // If the destination is negative reduce the width
        // image will cut off the side
        width += dest_start_x;
        // If there is no image to draw
        if (width <= 0)
        {
            return;
        }
        // The source start will also need to cut off the side
        source_start_x -= dest_start_x;
        // Reset the destination to 0
        dest_start_x = 0;
    }
    else
    {
        if ((g1->flags & G1_FLAG_RLE_COMPRESSION) && zoom_level > ZoomLevel{ 0 })
        {
            source_start_x -= dest_start_x & ~zoom_mask;
        }
    }

    int32_t dest_end_x = dest_start_x + width;

    if (dest_end_x > dpi->width)
    {
        // If the destination x is outside of the drawing area
        // reduce the image width.
        width -= dest_end_x - dpi->width;
        // If there is no image to draw.
        if (width <= 0)
            return;
    }

    dest_start_x = zoom_level.ApplyInversedTo(dest_start_x);

    uint8_t* dest_pointer = dpi->bits;
    // Move the pointer to the start point of the destination
    dest_pointer += (zoom_level.ApplyInversedTo(dpi->width) + dpi->pitch) * dest_start_y + dest_start_x;

    DrawSpriteArgs args(imageId, paletteMap, *g1, source_start_x, source_start_y, width, height, dest_pointer);
    GfxSpriteToBuffer(*dpi, args);
}

void FASTCALL GfxSpriteToBuffer(DrawPixelInfo& dpi, const DrawSpriteArgs& args)
{
    if (args.SourceImage.flags & G1_FLAG_RLE_COMPRESSION)
    {
        GfxRleSpriteToBuffer(dpi, args);
    }
    else if (!(args.SourceImage.flags & G1_FLAG_1))
    {
        GfxBmpSpriteToBuffer(dpi, args);
    }
}

/**
 * Draws the given colour image masked out by the given mask image. This can currently only cope with bitmap formatted mask and
 * colour images. Presumably the original game never used RLE images for masking. Colour 0 represents transparent.
 *
 *  rct2: 0x00681DE2
 */
void FASTCALL GfxDrawSpriteRawMaskedSoftware(
    DrawPixelInfo* dpi, const ScreenCoordsXY& scrCoords, const ImageId maskImage, const ImageId colourImage)
{
    int32_t left, top, right, bottom, width, height;
    auto imgMask = GfxGetG1Element(maskImage);
    auto imgColour = GfxGetG1Element(colourImage);
    if (imgMask == nullptr || imgColour == nullptr)
    {
        return;
    }

    // Must have transparency in order to pass check
    if (!(imgMask->flags & G1_FLAG_HAS_TRANSPARENCY) || !(imgColour->flags & G1_FLAG_HAS_TRANSPARENCY))
    {
        GfxDrawSpriteSoftware(dpi, colourImage, scrCoords);
        return;
    }

    if (dpi->zoom_level != ZoomLevel{ 0 })
    {
        // TODO: Implement other zoom levels (probably not used though)
        assert(false);
        return;
    }

    width = std::min(imgMask->width, imgColour->width);
    height = std::min(imgMask->height, imgColour->height);

    auto offsetCoords = scrCoords + ScreenCoordsXY{ imgMask->x_offset, imgMask->y_offset };

    left = std::max<int32_t>(dpi->x, offsetCoords.x);
    top = std::max<int32_t>(dpi->y, offsetCoords.y);
    right = std::min(dpi->x + dpi->width, offsetCoords.x + width);
    bottom = std::min(dpi->y + dpi->height, offsetCoords.y + height);

    width = right - left;
    height = bottom - top;
    if (width < 0 || height < 0)
        return;

    int32_t skipX = left - offsetCoords.x;
    int32_t skipY = top - offsetCoords.y;

    uint8_t const* maskSrc = imgMask->offset + (skipY * imgMask->width) + skipX;
    uint8_t const* colourSrc = imgColour->offset + (skipY * imgColour->width) + skipX;
    uint8_t* dst = dpi->bits + (left - dpi->x) + ((top - dpi->y) * (dpi->width + dpi->pitch));

    int32_t maskWrap = imgMask->width - width;
    int32_t colourWrap = imgColour->width - width;
    int32_t dstWrap = ((dpi->width + dpi->pitch) - width);

    MaskFn(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
}

const G1Element* GfxGetG1Element(const ImageId imageId)
{
    return GfxGetG1Element(imageId.GetIndex());
}

const G1Element* GfxGetG1Element(ImageIndex image_id)
{
    openrct2_assert(!gOpenRCT2NoGraphics, "GfxGetG1Element called on headless instance");

    auto offset = static_cast<size_t>(image_id);
    if (offset == 0x7FFFF || offset == ImageIndexUndefined)
    {
        return nullptr;
    }

    if (offset == SPR_TEMP)
    {
        return &_g1Temp;
    }

    if (offset < SPR_RCTC_G1_END)
    {
        if (offset < _g1.elements.size())
        {
            return &_g1.elements[offset];
        }
    }
    else if (offset < SPR_G2_END)
    {
        size_t idx = offset - SPR_G2_BEGIN;
        if (idx < _g2.header.num_entries)
        {
            return &_g2.elements[idx];
        }

        LOG_WARNING("Invalid entry in g2.dat requested, idx = %u. You may have to update your g2.dat.", idx);
    }
    else if (offset < SPR_CSG_END)
    {
        if (IsCsgLoaded())
        {
            size_t idx = offset - SPR_CSG_BEGIN;
            if (idx < _csg.header.num_entries)
            {
                return &_csg.elements[idx];
            }

            LOG_WARNING("Invalid entry in csg.dat requested, idx = %u.", idx);
        }
    }
    else if (offset < SPR_SCROLLING_TEXT_END)
    {
        size_t idx = offset - SPR_SCROLLING_TEXT_START;
        if (idx < std::size(_scrollingText))
        {
            return &_scrollingText[idx];
        }
    }
    else if (offset < SPR_IMAGE_LIST_END)
    {
        size_t idx = offset - SPR_IMAGE_LIST_BEGIN;
        if (idx < _imageListElements.size())
        {
            return &_imageListElements[idx];
        }
    }
    return nullptr;
}

void GfxSetG1Element(ImageIndex imageId, const G1Element* g1)
{
    bool isTemp = imageId == SPR_TEMP;
    bool isValid = (imageId >= SPR_IMAGE_LIST_BEGIN && imageId < SPR_IMAGE_LIST_END)
        || (imageId >= SPR_SCROLLING_TEXT_START && imageId < SPR_SCROLLING_TEXT_END);

#ifdef DEBUG
    openrct2_assert(!gOpenRCT2NoGraphics, "GfxSetG1Element called on headless instance");
    openrct2_assert(isValid || isTemp, "GfxSetG1Element called with unexpected image id");
    openrct2_assert(g1 != nullptr, "g1 was nullptr");
#endif

    if (g1 != nullptr)
    {
        if (isTemp)
        {
            _g1Temp = *g1;
        }
        else if (isValid)
        {
            if (imageId < SPR_RCTC_G1_END)
            {
                if (imageId < static_cast<ImageIndex>(_g1.elements.size()))
                {
                    _g1.elements[imageId] = *g1;
                }
            }
            else if (imageId < SPR_SCROLLING_TEXT_END)
            {
                size_t idx = static_cast<size_t>(imageId) - SPR_SCROLLING_TEXT_START;
                if (idx < std::size(_scrollingText))
                {
                    _scrollingText[idx] = *g1;
                }
            }
            else
            {
                size_t idx = static_cast<size_t>(imageId) - SPR_IMAGE_LIST_BEGIN;
                // Grow the element buffer if necessary
                while (idx >= _imageListElements.size())
                {
                    _imageListElements.resize(std::max<size_t>(256, _imageListElements.size() * 2));
                }
                _imageListElements[idx] = *g1;
            }
        }
    }
}

bool IsCsgLoaded()
{
    return _csgLoaded;
}

size_t G1CalculateDataSize(const G1Element* g1)
{
    if (g1->flags & G1_FLAG_PALETTE)
    {
        return g1->width * 3;
    }

    if (g1->flags & G1_FLAG_RLE_COMPRESSION)
    {
        if (g1->offset == nullptr)
        {
            return 0;
        }

        auto idx = (g1->height - 1) * 2;
        uint16_t offset = g1->offset[idx] | (g1->offset[idx + 1] << 8);
        uint8_t* ptr = g1->offset + offset;
        bool endOfLine = false;
        do
        {
            uint8_t chunk0 = *ptr++;
            ptr++; // offset
            uint8_t chunkSize = chunk0 & 0x7F;
            ptr += chunkSize;
            endOfLine = (chunk0 & 0x80) != 0;
        } while (!endOfLine);
        return ptr - g1->offset;
    }

    return g1->width * g1->height;
}
