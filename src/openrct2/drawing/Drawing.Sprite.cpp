/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../config/Config.h"
#include "../core/FileStream.h"
#include "../core/Guard.hpp"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../platform/Platform.h"
#include "../rct1/Csg.h"
#include "../sprites.h"
#include "../ui/UiContext.h"
#include "ScrollingText.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

/**
 * 12 elements from 0xF3 are the peep top colour, 12 elements from 0xCA are peep trouser colour
 *
 * rct2: 0x0009ABE0C
 */
// clang-format off
static thread_local uint8_t secondaryRemapPalette[256] = {
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
static thread_local uint8_t tertiaryRemapPalette[256] = {
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
            element.x_offset -= 1;
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
            element.y_offset += 1;
            break;
        case 25307:
        case 25315:
        case 25319:
            element.x_offset -= 1;
            element.y_offset += 1;
            break;
        case 25802:
            element.y_offset += 2;
            break;
    }
}

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
            for (auto i = 0u; i < SPR_PEEP_PICKUP_COUNT; ++i)
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
            if (_g2.elements[i].offset == nullptr)
            {
                _g2.elements[i].offset = _g2.data.get();
            }
            else
            {
                _g2.elements[i].offset += reinterpret_cast<uintptr_t>(_g2.data.get());
            }
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

    if (Config::Get().general.RCT1Path.empty())
    {
        LOG_VERBOSE("  unable to load CSG, RCT1 path not set");
        return false;
    }

    auto pathHeaderPath = FindCsg1idatAtLocation(Config::Get().general.RCT1Path);
    auto pathDataPath = FindCsg1datAtLocation(Config::Get().general.RCT1Path);
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
            if (_csg.elements[i].offset == nullptr)
            {
                _csg.elements[i].offset = _csg.data.get();
            }
            else
            {
                _csg.elements[i].offset += reinterpret_cast<uintptr_t>(_csg.data.get());
            }
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

    auto paletteMap = PaletteMap(secondaryRemapPalette);
    if (imageId.HasTertiary())
    {
        paletteMap = PaletteMap(tertiaryRemapPalette);
        auto tertiaryPaletteMap = GetPaletteMapForColour(imageId.GetTertiary());
        if (tertiaryPaletteMap.has_value())
        {
            paletteMap.Copy(
                kPaletteOffsetRemapTertiary, tertiaryPaletteMap.value(), kPaletteOffsetRemapPrimary, kPaletteLengthRemap);
        }
    }

    auto primaryPaletteMap = GetPaletteMapForColour(imageId.GetPrimary());
    if (primaryPaletteMap.has_value())
    {
        paletteMap.Copy(kPaletteOffsetRemapPrimary, primaryPaletteMap.value(), kPaletteOffsetRemapPrimary, kPaletteLengthRemap);
    }

    auto secondaryPaletteMap = GetPaletteMapForColour(imageId.GetSecondary());
    if (secondaryPaletteMap.has_value())
    {
        paletteMap.Copy(
            kPaletteOffsetRemapSecondary, secondaryPaletteMap.value(), kPaletteOffsetRemapPrimary, kPaletteLengthRemap);
    }

    return paletteMap;
}

void FASTCALL GfxDrawSpriteSoftware(DrawPixelInfo& dpi, const ImageId imageId, const ScreenCoordsXY& spriteCoords)
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
    DrawPixelInfo& dpi, const ImageId imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap)
{
    const auto zoomLevel = dpi.zoom_level;
    int32_t x = coords.x;
    int32_t y = coords.y;

    const auto* g1 = GfxGetG1Element(imageId);
    if (g1 == nullptr)
    {
        return;
    }

    if (zoomLevel > ZoomLevel{ 0 } && (g1->flags & G1_FLAG_HAS_ZOOM_SPRITE))
    {
        DrawPixelInfo zoomed_dpi = dpi;
        zoomed_dpi.bits = dpi.bits;
        zoomed_dpi.x = dpi.x;
        zoomed_dpi.y = dpi.y;
        zoomed_dpi.height = dpi.height;
        zoomed_dpi.width = dpi.width;
        zoomed_dpi.pitch = dpi.pitch;
        zoomed_dpi.zoom_level = zoomLevel - 1;

        const auto spriteCoords = ScreenCoordsXY{ coords.x / 2, coords.y / 2 };
        GfxDrawSpritePaletteSetSoftware(
            zoomed_dpi, imageId.WithIndex(imageId.GetIndex() - g1->zoomed_offset), spriteCoords, paletteMap);
        return;
    }

    if (zoomLevel > ZoomLevel{ 0 } && (g1->flags & G1_FLAG_NO_ZOOM_DRAW))
    {
        return;
    }

    // mber: There should not be two separate code paths for minifying and magnifying sprites.
    //       I haven't been able to refactor the code in a way that handles both cases properly with one code path.
    //       For the moment, I've added this block here just for magnification with the old code continuing below.
    if (zoomLevel < ZoomLevel{ 0 })
    {
        ScreenCoordsXY spriteTopLeft = { zoomLevel.ApplyInversedTo(coords.x + g1->x_offset),
                                         zoomLevel.ApplyInversedTo(coords.y + g1->y_offset) };

        ScreenCoordsXY spriteBottomLeft{ zoomLevel.ApplyInversedTo(coords.x + g1->x_offset + g1->width),
                                         zoomLevel.ApplyInversedTo(coords.y + g1->y_offset + g1->height) };

        const int32_t width = std::min(spriteBottomLeft.x, dpi.x + dpi.width) - std::max(spriteTopLeft.x, dpi.x);
        const int32_t height = std::min(spriteBottomLeft.y, dpi.y + dpi.height) - std::max(spriteTopLeft.y, dpi.y);

        if (width <= 0 || height <= 0)
            return;

        const int32_t offsetX = dpi.x - spriteTopLeft.x;
        const int32_t offsetY = dpi.y - spriteTopLeft.y;
        const int32_t srcX = std::max(0, offsetX);
        const int32_t srcY = std::max(0, offsetY);
        uint8_t* dst = dpi.bits + std::max(0, -offsetX) + std::max(0, -offsetY) * dpi.LineStride();

        DrawSpriteArgs args(imageId, paletteMap, *g1, srcX, srcY, width, height, dst);
        GfxSpriteToBuffer(dpi, args);
        return;
    }

    const int32_t zoom_mask = zoomLevel > ZoomLevel{ 0 } ? zoomLevel.ApplyTo(0xFFFFFFFF) : 0xFFFFFFFF;

    if (zoomLevel > ZoomLevel{ 0 } && g1->flags & G1_FLAG_RLE_COMPRESSION)
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
        dest_start_y -= dpi.WorldY();
    }
    else
    {
        dest_start_y = (dest_start_y & zoom_mask) - dpi.WorldY();
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
        if ((g1->flags & G1_FLAG_RLE_COMPRESSION) && zoomLevel > ZoomLevel{ 0 })
        {
            source_start_y -= dest_start_y & ~zoom_mask;
            height += dest_start_y & ~zoom_mask;
        }
    }

    int32_t dest_end_y = dest_start_y + height;

    if (dest_end_y > dpi.WorldHeight())
    {
        // If the destination y is outside of the drawing
        // image reduce the height of the image
        height -= dest_end_y - dpi.WorldHeight();
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
    int16_t dest_start_x = ((x + g1->x_offset + ~zoom_mask) & zoom_mask) - dpi.WorldX();

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
        if ((g1->flags & G1_FLAG_RLE_COMPRESSION) && zoomLevel > ZoomLevel{ 0 })
        {
            source_start_x -= dest_start_x & ~zoom_mask;
        }
    }

    int32_t dest_end_x = dest_start_x + width;

    if (dest_end_x > dpi.WorldWidth())
    {
        // If the destination x is outside of the drawing area
        // reduce the image width.
        width -= dest_end_x - dpi.WorldWidth();
        // If there is no image to draw.
        if (width <= 0)
            return;
    }

    dest_start_x = zoomLevel.ApplyInversedTo(dest_start_x);

    uint8_t* dest_pointer = dpi.bits;
    // Move the pointer to the start point of the destination
    dest_pointer += (zoomLevel.ApplyInversedTo(dpi.WorldWidth()) + dpi.pitch) * dest_start_y + dest_start_x;

    DrawSpriteArgs args(imageId, paletteMap, *g1, source_start_x, source_start_y, width, height, dest_pointer);
    GfxSpriteToBuffer(dpi, args);
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
    DrawPixelInfo& dpi, const ScreenCoordsXY& scrCoords, const ImageId maskImage, const ImageId colourImage)
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

    ZoomLevel zoom = dpi.zoom_level;
    if (dpi.zoom_level > ZoomLevel{ 0 })
    {
        assert(false);
    }

    width = zoom.ApplyInversedTo(std::min(imgMask->width, imgColour->width));
    height = zoom.ApplyInversedTo(std::min(imgMask->height, imgColour->height));

    ScreenCoordsXY offsetCoords = scrCoords + ScreenCoordsXY{ imgMask->x_offset, imgMask->y_offset };
    offsetCoords.x = zoom.ApplyInversedTo(offsetCoords.x);
    offsetCoords.y = zoom.ApplyInversedTo(offsetCoords.y);

    left = std::max(dpi.x, offsetCoords.x);
    top = std::max(dpi.y, offsetCoords.y);
    right = std::min(dpi.x + dpi.width, offsetCoords.x + width);
    bottom = std::min(dpi.y + dpi.height, offsetCoords.y + height);

    width = right - left;
    height = bottom - top;
    if (width < 0 || height < 0)
        return;

    uint8_t* dst = dpi.bits + (left - dpi.x) + ((top - dpi.y) * dpi.LineStride());
    int32_t skipX = left - offsetCoords.x;
    int32_t skipY = top - offsetCoords.y;
    if (zoom < ZoomLevel{ 0 })
    {
        MaskMagnify(
            zoom, width, height, imgMask->offset, imgColour->offset, dst, imgMask->width, imgColour->width, dpi.LineStride(),
            skipX, skipY);
        return;
    }

    uint8_t const* maskSrc = imgMask->offset + (skipY * imgMask->width) + skipX;
    uint8_t const* colourSrc = imgColour->offset + (skipY * imgColour->width) + skipX;

    int32_t maskWrap = imgMask->width - width;
    int32_t colourWrap = imgColour->width - width;
    int32_t dstWrap = dpi.LineStride() - width;

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
