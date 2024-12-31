/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "ImageId.hpp"

#include <cstddef>
#include <cstdint>
#include <list>

struct G1Element;

struct ImageList
{
    ImageIndex BaseId{};
    ImageIndex Count{};

    ImageList() = default;
    ImageList(ImageIndex baseId, ImageIndex count)
        : BaseId(baseId)
        , Count(count)
    {
    }

    bool Contains(ImageIndex index) const
    {
        return index >= BaseId && index < GetEnd();
    }

    ImageIndex GetEnd() const
    {
        return BaseId + Count;
    }

    static ImageList FromBeginEnd(ImageIndex begin, ImageIndex end)
    {
        return ImageList(begin, end - begin);
    }
};

constexpr bool operator==(const ImageList& lhs, const ImageList& rhs)
{
    return lhs.BaseId == rhs.BaseId && lhs.Count == rhs.Count;
}

constexpr bool operator!=(const ImageList& lhs, const ImageList& rhs)
{
    return !(lhs == rhs);
}

uint32_t GfxObjectAllocateImages(const G1Element* images, uint32_t count);
void GfxObjectFreeImages(uint32_t baseImageId, uint32_t count);
void GfxObjectCheckAllImagesFreed();
size_t ImageListGetUsedCount();
size_t ImageListGetMaximum();
const std::list<ImageList>& GetAvailableAllocationRanges();
