/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../SpriteIds.h"
#include "../config/Config.h"
#include "../core/FileStream.h"
#include "../core/Guard.hpp"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../platform/Platform.h"
#include "../rct1/Csg.h"
#include "../ui/UiContext.h"
#include "ScrollingText.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

static constexpr uint8_t kPaletteLengthRemap = 12;

/**
 * 12 elements from 0xF3 are the peep top colour, 12 elements from 0xCA are peep trouser colour
 *
 * rct2: 0x0009ABE0C
 */
// clang-format off
static thread_local uint8_t kSecondaryRemapPalette[256] = {
    0x00, 0xF3, 0xF4, 0xF5, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

/** rct2: 0x009ABF0C */
static thread_local uint8_t kTertiaryRemapPalette[256] = {
    0x00, 0xF3, 0xF4, 0xF5, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

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

static void OverrideElementOffsets(size_t index, G1Element& element)
{
    switch (index)
    {
        case 25285:
            element.xOffset -= 1;
            break;
        case 25286:
        case 25317:
        case 25318:
        case 25323:
        case 25324:
        case 25325:
        case 25326:
        case 25455:
        case 25456:
        case 25457:
        case 25458:
        case 25459:
        case 25460:
        case 25461:
        case 25462:
        case 25463:
        case 25464:
        case 25465:
        case 25466:
        case 25467:
        case 25468:
        case 25469:
        case 25470:
        case 25471:
        case 25472:
        case 25473:
        case 25474:
        case 25475:
        case 25476:
        case 25521:
        case 25522:
        case 25523:
        case 25524:
        case 25525:
        case 25526:
        case 25527:
        case 25528:
        case 25529:
        case 25530:
        case 25531:
        case 25532:
        case 25533:
        case 25534:
        case 25659:
        case 25660:
        case 25661:
        case 25662:
        case 25663:
        case 25664:
        case 25665:
        case 25666:
        case 25667:
        case 25668:
        case 25669:
        case 25670:
        case 25671:
        case 25672:
        case 25673:
        case 25674:
        case 25675:
        case 25676:
        case 25677:
        case 25678:
        case 25679:
        case 25680:
        case 25681:
        case 25682:
        case 25683:
        case 25684:
        case 25685:
        case 25686:
        case 25687:
        case 25688:
        case 25689:
        case 25690:
        case 25719:
        case 25721:
        case 25723:
        case 25725:
        case 25727:
        case 25728:
        case 25730:
        case 25732:
        case 25733:
        case 25735:
        case 25737:
        case 25738:
        case 25740:
        case 25742:
        case 25743:
        case 25745:
        case 25747:
        case 25748:
        case 25781:
        case 25782:
        case 25783:
        case 25784:
        case 25785:
        case 25786:
        case 25787:
        case 25788:
        case 25789:
        case 25790:
        case 25791:
        case 25792:
        case 25793:
        case 25794:
        case 25795:
        case 25796:
        case 25797:
        case 25798:
        case 25799:
        case 25800:
        case 25801:
        case 25803:
        case 25804:
        case 25805:
        case 25806:
        case 25807:
        case 25808:
        case 25809:
        case 25810:
        case 25811:
        case 25812:
        case 25813:
        case 25814:
        case 25815:
        case 25816:
        case 25817:
        case 25818:
        case 25819:
        case 25820:
        case 25821:
        case 25822:
        case 25823:
        case 25824:
        case 25825:
        case 25826:
        case 25827:
        case 25828:
        case 25829:
        case 25830:
        case 25831:
        case 25832:
        case 25833:
        case 25834:
        case 25835:
        case 25836:
        case 25837:
        case 25838:
        case 25839:
        case 25840:
        case 25841:
        case 25842:
        case 25843:
        case 25844:
        case 25845:
        case 25846:
        case 25847:
        case 25848:
        case 25849:
        case 25850:
        case 25851:
        case 25852:
            element.yOffset += 1;
            break;
        case 25307:
        case 25315:
        case 25319:
            element.xOffset -= 1;
            element.yOffset += 1;
            break;
        case 25802:
            element.yOffset += 2;
            break;
    }
}

static void ReadAndConvertGxDat(IStream* stream, size_t count, bool is_rctc, G1Element* elements)
{
    auto g1Elements32 = std::make_unique<StoredG1Element[]>(count);
    stream->Read(g1Elements32.get(), count * sizeof(StoredG1Element));
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

            const StoredG1Element& src = g1Elements32[rctc];

            // Double cast to silence compiler warning about casting to
            // pointer from integer of mismatched length.
            elements[i].offset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(src.offset));
            elements[i].width = src.width;
            elements[i].height = src.height;
            elements[i].xOffset = src.xOffset;
            elements[i].yOffset = src.yOffset;
            elements[i].flags = src.flags;

            if (src.flags.has(G1Flag::hasZoomSprite))
            {
                elements[i].zoomedOffset = static_cast<int32_t>(i - rctc_to_rct2_index(rctc - src.zoomedOffset));
            }
            else
            {
                elements[i].zoomedOffset = src.zoomedOffset;
            }

            ++rctc;
        }

        // The pincer graphic for picking up peeps is different in
        // RCTC, and the sprites have different offsets to accommodate
        // the change. This reverts the offsets to their RCT2 values.
        for (const auto& animation : sprite_peep_pickup_starts)
        {
            for (auto i = 0u; i < SPR_PEEP_PICKUP_COUNT; ++i)
            {
                elements[animation.start + i].xOffset -= animation.x_offset;
                elements[animation.start + i].yOffset -= animation.y_offset;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < count; i++)
        {
            const StoredG1Element& src = g1Elements32[i];

            // Double cast to silence compiler warning about casting to
            // pointer from integer of mismatched length.
            elements[i].offset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(src.offset));
            elements[i].width = src.width;
            elements[i].height = src.height;
            elements[i].xOffset = src.xOffset;
            elements[i].yOffset = src.yOffset;
            elements[i].flags = src.flags;
            elements[i].zoomedOffset = src.zoomedOffset;
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

static void MaskMagnify(
    const ZoomLevel zoom, int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc,
    uint8_t* RESTRICT dst, int32_t maskStride, int32_t colourStride, int32_t dstStride, int32_t srcX, int32_t srcY)
{
    for (int32_t y = 0; y < height; y++)
    {
        auto nextDst = dst + dstStride;
        for (int32_t x = 0; x < width; x++, dst++)
        {
            auto srcMask = maskSrc + (maskStride * zoom.ApplyTo(srcY + y) + zoom.ApplyTo(srcX + x));
            auto srcColour = colourSrc + (colourStride * zoom.ApplyTo(srcY + y) + zoom.ApplyTo(srcX + x));
            const uint8_t colour = (*srcColour) & (*srcMask);
            if (colour != 0)
            {
                *dst = colour;
            }
        }
        dst = nextDst;
    }
}

static Gx _g1 = {};
static Gx _g2 = {};
static Gx _fonts = {};
static Gx _palettes = {};
static Gx _tracks = {};
static Gx _csg = {};
static G1Element _scrollingText[ScrollingText::kMaxEntries]{};
static bool _csgLoaded = false;

static G1Element _g1Temp = {};
static std::vector<G1Element> _imageListElements;

/**
 *
 *  rct2: 0x00678998
 */
bool GfxLoadG1(const IPlatformEnvironment& env)
{
    LOG_VERBOSE("GfxLoadG1(...)");
    try
    {
        auto path = env.FindFile(DirBase::rct2, DirId::data, u8"g1.dat");
        auto fs = FileStream(path, FileMode::open);
        _g1.header = fs.ReadValue<G1Header>();

        LOG_VERBOSE("g1.dat, number of entries: %u", _g1.header.numEntries);

        if (_g1.header.numEntries < SPR_G1_END)
        {
            throw std::runtime_error("Not enough elements in g1.dat");
        }

        // Read element headers
        bool is_rctc = _g1.header.numEntries == SPR_RCTC_G1_END;
        _g1.elements.resize(_g1.header.numEntries);
        ReadAndConvertGxDat(&fs, _g1.header.numEntries, is_rctc, _g1.elements.data());

        // Read element data
        _g1.data = fs.ReadArray<uint8_t>(_g1.header.totalSize);

        // Fix entry data offsets
        for (uint32_t i = 0; i < _g1.header.numEntries; i++)
        {
            if (_g1.elements[i].offset == nullptr)
            {
                _g1.elements[i].offset = _g1.data.get();
            }
            else
            {
                _g1.elements[i].offset += reinterpret_cast<uintptr_t>(_g1.data.get());
            }
            OverrideElementOffsets(i, _g1.elements[i]);
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
            auto& uiContext = GetContext()->GetUiContext();
            uiContext.ShowMessageBox("Unable to load g1.dat. Your RollerCoaster Tycoon 2 path may be incorrectly set.");
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

void GfxUnloadG2PalettesFontsTracks()
{
    _g2.data.reset();
    _g2.elements.clear();
    _g2.elements.shrink_to_fit();

    _palettes.data.reset();
    _palettes.elements.clear();
    _palettes.elements.shrink_to_fit();

    _fonts.data.reset();
    _fonts.elements.clear();
    _fonts.elements.shrink_to_fit();

    _tracks.data.reset();
    _tracks.elements.clear();
    _tracks.elements.shrink_to_fit();
}

void GfxUnloadCsg()
{
    _csg.data.reset();
    _csg.elements.clear();
    _csg.elements.shrink_to_fit();
    _csgLoaded = false;
}

static bool GfxLoadOpenRCT2Gx(std::string filename, Gx& target, size_t expectedNumItems)
{
    LOG_VERBOSE("GfxLoadOpenRCT2Gx(\"%s\")", filename.c_str());
    auto& env = GetContext()->GetPlatformEnvironment();

    std::string path = Path::Combine(env.GetDirectoryPath(DirBase::openrct2), filename.c_str());

    try
    {
        auto fs = FileStream(path, FileMode::open);
        target.header = fs.ReadValue<G1Header>();

        // Read element headers
        target.elements.resize(target.header.numEntries);
        ReadAndConvertGxDat(&fs, target.header.numEntries, false, target.elements.data());

        // Read element data
        target.data = fs.ReadArray<uint8_t>(target.header.totalSize);

        if (target.header.numEntries != expectedNumItems)
        {
            std::string errorMessage = "Mismatched " + filename + " size.\nExpected: " + std::to_string(expectedNumItems)
                + "\nActual: " + std::to_string(target.header.numEntries) + "\n" + filename
                + " may be installed improperly.\nPath to " + filename + ": " + path;

            LOG_ERROR(errorMessage.c_str());

            if (!gOpenRCT2Headless)
            {
                auto& uiContext = GetContext()->GetUiContext();
                uiContext.ShowMessageBox(errorMessage);
                uiContext.ShowMessageBox(
                    "Warning: You may experience graphical glitches if you continue. It's recommended "
                    "that you update "
                    + filename + " if you're seeing this message");
            }
        }

        // Fix entry data offsets
        for (uint32_t i = 0; i < target.header.numEntries; i++)
        {
            if (target.elements[i].offset == nullptr)
            {
                target.elements[i].offset = target.data.get();
            }
            else
            {
                target.elements[i].offset += reinterpret_cast<uintptr_t>(target.data.get());
            }
        }
        return true;
    }
    catch (const std::exception&)
    {
        target.elements.clear();
        target.elements.shrink_to_fit();

        LOG_FATAL("Unable to load %s graphics", filename.c_str());
        if (!gOpenRCT2Headless)
        {
            auto& uiContext = GetContext()->GetUiContext();
            uiContext.ShowMessageBox("Unable to load " + filename);
        }
    }
    return false;
}

void GfxLoadG2PalettesFontsTracks()
{
    GfxLoadOpenRCT2Gx("g2.dat", _g2, kG2SpriteCount);
    GfxLoadOpenRCT2Gx("palettes.dat", _palettes, kPalettesDatSpriteCount);
    GfxLoadOpenRCT2Gx("fonts.dat", _fonts, kFontsDatSpriteCount);
    GfxLoadOpenRCT2Gx("tracks.dat", _tracks, kTracksDatSpriteCount);
}

bool GfxLoadCsg()
{
    LOG_VERBOSE("GfxLoadCsg()");

    if (Config::Get().general.rct1Path.empty())
    {
        LOG_VERBOSE("  unable to load CSG, RCT1 path not set");
        return false;
    }

    auto pathHeaderPath = FindCsg1idatAtLocation(Config::Get().general.rct1Path);
    auto pathDataPath = FindCsg1datAtLocation(Config::Get().general.rct1Path);
    try
    {
        auto fileHeader = FileStream(pathHeaderPath, FileMode::open);
        auto fileData = FileStream(pathDataPath, FileMode::open);
        size_t fileHeaderSize = fileHeader.GetLength();
        size_t fileDataSize = fileData.GetLength();

        _csg.header.numEntries = static_cast<uint32_t>(fileHeaderSize / sizeof(StoredG1Element));
        _csg.header.totalSize = static_cast<uint32_t>(fileDataSize);

        if (!CsgIsUsable(_csg))
        {
            LOG_WARNING("Cannot load CSG1.DAT, it has too few entries. Only CSG1.DAT from Loopy Landscapes will work.");
            return false;
        }

        // Read element headers
        _csg.elements.resize(_csg.header.numEntries);
        ReadAndConvertGxDat(&fileHeader, _csg.header.numEntries, false, _csg.elements.data());

        // Read element data
        _csg.data = fileData.ReadArray<uint8_t>(_csg.header.totalSize);

        // Fix entry data offsets
        for (uint32_t i = 0; i < _csg.header.numEntries; i++)
        {
            if (_csg.elements[i].offset == nullptr)
            {
                _csg.elements[i].offset = _csg.data.get();
            }
            else
            {
                _csg.elements[i].offset += reinterpret_cast<uintptr_t>(_csg.data.get());
            }
            // RCT1 used zoomed offsets that counted from the beginning of the file, rather than from the current sprite.
            if (_csg.elements[i].flags.has(G1Flag::hasZoomSprite))
            {
                _csg.elements[i].zoomedOffset = i - _csg.elements[i].zoomedOffset;
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
        MemoryStream istream(buffer.data(), buffer.size());
        Gx gx;

        gx.header = istream.ReadValue<G1Header>();

        // Read element headers
        gx.elements.resize(gx.header.numEntries);
        ReadAndConvertGxDat(&istream, gx.header.numEntries, false, gx.elements.data());

        // Read element data
        gx.data = istream.ReadArray<uint8_t>(gx.header.totalSize);

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
        return GetPaletteMapForColour(static_cast<FilterPaletteID>(paletteId));
    }

    auto paletteMap = PaletteMap(std::span(reinterpret_cast<PaletteIndex*>(kSecondaryRemapPalette), 256));
    if (imageId.HasTertiary())
    {
        paletteMap = PaletteMap(std::span(reinterpret_cast<PaletteIndex*>(kTertiaryRemapPalette), 256));
        auto tertiaryPaletteMap = GetPaletteMapForColour(static_cast<FilterPaletteID>(imageId.GetTertiary()));
        if (tertiaryPaletteMap.has_value())
        {
            paletteMap.Copy(
                PaletteIndex::tertiaryRemap0, tertiaryPaletteMap.value(), PaletteIndex::primaryRemap0, kPaletteLengthRemap);
        }
    }

    auto primaryPaletteMap = GetPaletteMapForColour(static_cast<FilterPaletteID>(imageId.GetPrimary()));
    if (primaryPaletteMap.has_value())
    {
        paletteMap.Copy(
            PaletteIndex::primaryRemap0, primaryPaletteMap.value(), PaletteIndex::primaryRemap0, kPaletteLengthRemap);
    }

    auto secondaryPaletteMap = GetPaletteMapForColour(static_cast<FilterPaletteID>(imageId.GetSecondary()));
    if (secondaryPaletteMap.has_value())
    {
        paletteMap.Copy(
            PaletteIndex::secondaryRemap0, secondaryPaletteMap.value(), PaletteIndex::primaryRemap0, kPaletteLengthRemap);
    }

    return paletteMap;
}

void FASTCALL GfxDrawSpriteSoftware(RenderTarget& rt, const ImageId imageId, const ScreenCoordsXY& spriteCoords)
{
    if (imageId.HasValue())
    {
        auto palette = GfxDrawSpriteGetPalette(imageId);
        if (!palette)
        {
            palette = PaletteMap::GetDefault();
        }
        GfxDrawSpritePaletteSetSoftware(rt, imageId, spriteCoords, *palette);
    }
}

/*
 * rct: 0x0067A46E
 * image_id (ebx) and also (0x00EDF81C)
 * palette_pointer (0x9ABDA4)
 * unknown_pointer (0x9E3CDC)
 * rt (edi)
 * x (cx)
 * y (dx)
 */
void FASTCALL GfxDrawSpritePaletteSetSoftware(
    RenderTarget& rt, const ImageId imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap)
{
    const auto zoomLevel = rt.zoom_level;
    int32_t x = coords.x;
    int32_t y = coords.y;

    const auto* g1 = GfxGetG1Element(imageId);
    if (g1 == nullptr)
    {
        return;
    }

    if (zoomLevel > ZoomLevel{ 0 } && g1->flags.has(G1Flag::hasZoomSprite))
    {
        RenderTarget zoomedRT = rt;
        zoomedRT.bits = rt.bits;
        zoomedRT.x = rt.x;
        zoomedRT.y = rt.y;
        zoomedRT.height = rt.height;
        zoomedRT.width = rt.width;
        zoomedRT.pitch = rt.pitch;
        zoomedRT.zoom_level = zoomLevel - 1;

        const auto spriteCoords = ScreenCoordsXY{ coords.x / 2, coords.y / 2 };
        GfxDrawSpritePaletteSetSoftware(
            zoomedRT, imageId.WithIndex(imageId.GetIndex() - g1->zoomedOffset), spriteCoords, paletteMap);
        return;
    }

    if (zoomLevel > ZoomLevel{ 0 } && g1->flags.has(G1Flag::noZoomDraw))
    {
        return;
    }

    // mber: There should not be two separate code paths for minifying and magnifying sprites.
    //       I haven't been able to refactor the code in a way that handles both cases properly with one code path.
    //       For the moment, I've added this block here just for magnification with the old code continuing below.
    if (zoomLevel < ZoomLevel{ 0 })
    {
        ScreenCoordsXY spriteTopLeft = { zoomLevel.ApplyInversedTo(coords.x + g1->xOffset),
                                         zoomLevel.ApplyInversedTo(coords.y + g1->yOffset) };

        ScreenCoordsXY spriteBottomLeft{ zoomLevel.ApplyInversedTo(coords.x + g1->xOffset + g1->width),
                                         zoomLevel.ApplyInversedTo(coords.y + g1->yOffset + g1->height) };

        const int32_t width = std::min(spriteBottomLeft.x, rt.x + rt.width) - std::max(spriteTopLeft.x, rt.x);
        const int32_t height = std::min(spriteBottomLeft.y, rt.y + rt.height) - std::max(spriteTopLeft.y, rt.y);

        if (width <= 0 || height <= 0)
            return;

        const int32_t offsetX = rt.x - spriteTopLeft.x;
        const int32_t offsetY = rt.y - spriteTopLeft.y;
        const int32_t srcX = std::max(0, offsetX);
        const int32_t srcY = std::max(0, offsetY);
        uint8_t* dst = rt.bits + std::max(0, -offsetX) + std::max(0, -offsetY) * rt.LineStride();

        DrawSpriteArgs args(imageId, paletteMap, *g1, srcX, srcY, width, height, dst);
        GfxSpriteToBuffer(rt, args);
        return;
    }

    const int32_t zoom_mask = zoomLevel > ZoomLevel{ 0 } ? zoomLevel.ApplyTo(0xFFFFFFFF) : 0xFFFFFFFF;

    if (zoomLevel > ZoomLevel{ 0 } && g1->flags.has(G1Flag::hasRLECompression))
    {
        x -= ~zoom_mask;
        y -= ~zoom_mask;
    }

    // This will be the height of the drawn image
    int32_t height = g1->height;

    // This is the start y coordinate on the destination
    int16_t dest_start_y = y + g1->yOffset;

    // For whatever reason the RLE version does not use
    // the zoom mask on the y coordinate but does on x.
    if (g1->flags.has(G1Flag::hasRLECompression))
    {
        dest_start_y -= rt.WorldY();
    }
    else
    {
        dest_start_y = (dest_start_y & zoom_mask) - rt.WorldY();
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
        if (g1->flags.has(G1Flag::hasRLECompression) && zoomLevel > ZoomLevel{ 0 })
        {
            source_start_y -= dest_start_y & ~zoom_mask;
            height += dest_start_y & ~zoom_mask;
        }
    }

    int32_t dest_end_y = dest_start_y + height;

    if (dest_end_y > rt.WorldHeight())
    {
        // If the destination y is outside of the drawing
        // image reduce the height of the image
        height -= dest_end_y - rt.WorldHeight();
    }
    // If the image no longer has anything to draw
    if (height <= 0)
        return;

    dest_start_y = zoomLevel.ApplyInversedTo(dest_start_y);

    // This will be the width of the drawn image
    int32_t width = g1->width;

    // This is the source start x coordinate
    int32_t source_start_x = 0;
    // This is the destination start x coordinate
    int16_t dest_start_x = ((x + g1->xOffset + ~zoom_mask) & zoom_mask) - rt.WorldX();

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
        if (g1->flags.has(G1Flag::hasRLECompression) && zoomLevel > ZoomLevel{ 0 })
        {
            source_start_x -= dest_start_x & ~zoom_mask;
        }
    }

    int32_t dest_end_x = dest_start_x + width;

    if (dest_end_x > rt.WorldWidth())
    {
        // If the destination x is outside of the drawing area
        // reduce the image width.
        width -= dest_end_x - rt.WorldWidth();
        // If there is no image to draw.
        if (width <= 0)
            return;
    }

    dest_start_x = zoomLevel.ApplyInversedTo(dest_start_x);

    uint8_t* dest_pointer = rt.bits;
    // Move the pointer to the start point of the destination
    dest_pointer += (zoomLevel.ApplyInversedTo(rt.WorldWidth()) + rt.pitch) * dest_start_y + dest_start_x;

    DrawSpriteArgs args(imageId, paletteMap, *g1, source_start_x, source_start_y, width, height, dest_pointer);
    GfxSpriteToBuffer(rt, args);
}

void FASTCALL GfxSpriteToBuffer(RenderTarget& rt, const DrawSpriteArgs& args)
{
    if (args.SourceImage.flags.has(G1Flag::hasRLECompression))
    {
        GfxRleSpriteToBuffer(rt, args);
    }
    else if (!args.SourceImage.flags.has(G1Flag::one))
    {
        GfxBmpSpriteToBuffer(rt, args);
    }
}

/**
 * Draws the given colour image masked out by the given mask image. This can currently only cope with bitmap formatted mask and
 * colour images. Presumably the original game never used RLE images for masking. Colour 0 represents transparent.
 *
 *  rct2: 0x00681DE2
 */
void FASTCALL GfxDrawSpriteRawMaskedSoftware(
    RenderTarget& rt, const ScreenCoordsXY& scrCoords, const ImageId maskImage, const ImageId colourImage)
{
    int32_t left, top, right, bottom, width, height;
    auto imgMask = GfxGetG1Element(maskImage);
    auto imgColour = GfxGetG1Element(colourImage);
    if (imgMask == nullptr || imgColour == nullptr)
    {
        return;
    }

    // Must have transparency in order to pass check
    if (!imgMask->flags.has(G1Flag::hasTransparency) || !imgColour->flags.has(G1Flag::hasTransparency))
    {
        GfxDrawSpriteSoftware(rt, colourImage, scrCoords);
        return;
    }

    ZoomLevel zoom = rt.zoom_level;
    if (rt.zoom_level > ZoomLevel{ 0 })
    {
        assert(false);
    }

    width = zoom.ApplyInversedTo(std::min(imgMask->width, imgColour->width));
    height = zoom.ApplyInversedTo(std::min(imgMask->height, imgColour->height));

    ScreenCoordsXY offsetCoords = scrCoords + ScreenCoordsXY{ imgMask->xOffset, imgMask->yOffset };
    offsetCoords.x = zoom.ApplyInversedTo(offsetCoords.x);
    offsetCoords.y = zoom.ApplyInversedTo(offsetCoords.y);

    left = std::max(rt.x, offsetCoords.x);
    top = std::max(rt.y, offsetCoords.y);
    right = std::min(rt.x + rt.width, offsetCoords.x + width);
    bottom = std::min(rt.y + rt.height, offsetCoords.y + height);

    width = right - left;
    height = bottom - top;
    if (width < 0 || height < 0)
        return;

    uint8_t* dst = rt.bits + (left - rt.x) + ((top - rt.y) * rt.LineStride());
    int32_t skipX = left - offsetCoords.x;
    int32_t skipY = top - offsetCoords.y;
    if (zoom < ZoomLevel{ 0 })
    {
        MaskMagnify(
            zoom, width, height, imgMask->offset, imgColour->offset, dst, imgMask->width, imgColour->width, rt.LineStride(),
            skipX, skipY);
        return;
    }

    uint8_t const* maskSrc = imgMask->offset + (skipY * imgMask->width) + skipX;
    uint8_t const* colourSrc = imgColour->offset + (skipY * imgColour->width) + skipX;

    int32_t maskWrap = imgMask->width - width;
    int32_t colourWrap = imgColour->width - width;
    int32_t dstWrap = rt.LineStride() - width;

    MaskFn(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
}

const G1Element* GfxGetG1Element(const ImageId imageId)
{
    return GfxGetG1Element(imageId.GetIndex());
}

const G1Element* GfxGetG1Element(ImageIndex image_id)
{
    Guard::Assert(!gOpenRCT2NoGraphics, "GfxGetG1Element called on headless instance");

    auto offset = static_cast<size_t>(image_id);
    if (offset == kImageIndexUndefined)
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
        if (idx < _g2.header.numEntries)
        {
            return &_g2.elements[idx];
        }

        LOG_WARNING("Invalid entry in g2.dat requested, idx = %u. You may have to update your g2.dat.", idx);
    }
    else if (offset < SPR_PALETTE_END)
    {
        size_t idx = offset - SPR_PALETTE_START;
        if (idx < _palettes.header.numEntries)
        {
            return &_palettes.elements[idx];
        }

        LOG_WARNING("Invalid entry in palettes.dat requested, idx = %u. You may have to update your palettes.dat.", idx);
    }
    else if (offset < SPR_FONTS_END)
    {
        size_t idx = offset - SPR_FONTS_BEGIN;
        if (idx < _fonts.header.numEntries)
        {
            return &_fonts.elements[idx];
        }

        LOG_WARNING("Invalid entry in fonts.dat requested, idx = %u. You may have to update your fonts.dat.", idx);
    }
    else if (offset < SPR_TRACKS_END)
    {
        size_t idx = offset - SPR_TRACKS_BEGIN;
        if (idx < _tracks.header.numEntries)
        {
            return &_tracks.elements[idx];
        }

        LOG_WARNING("Invalid entry in tracks.dat requested, idx = %u. You may have to update your tracks.dat.", idx);
    }
    else if (offset < SPR_CSG_END)
    {
        if (IsCsgLoaded())
        {
            size_t idx = offset - SPR_CSG_BEGIN;
            if (idx < _csg.header.numEntries)
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

const G1Palette* GfxGetG1Palette(ImageIndex imageId)
{
    const auto* element = GfxGetG1Element(imageId);
    if (element == nullptr)
        return nullptr;

    return element->asPalette();
}

void GfxSetG1Element(ImageIndex imageId, const G1Element* g1)
{
    bool isTemp = imageId == SPR_TEMP;
    bool isValid = (imageId >= SPR_IMAGE_LIST_BEGIN && imageId < SPR_IMAGE_LIST_END)
        || (imageId >= SPR_SCROLLING_TEXT_START && imageId < SPR_SCROLLING_TEXT_END);

#ifdef DEBUG
    Guard::Assert(!gOpenRCT2NoGraphics, "GfxSetG1Element called on headless instance");
    Guard::Assert(isValid || isTemp, "GfxSetG1Element called with unexpected image id");
    Guard::Assert(g1 != nullptr, "g1 was nullptr");
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
    if (const auto* asPalette = g1->asPalette())
    {
        return asPalette->numColours * 3;
    }

    if (g1->flags.has(G1Flag::hasRLECompression))
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
