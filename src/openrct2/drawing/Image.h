/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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

struct rct_g1_element;

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

uint32_t gfx_object_allocate_images(const rct_g1_element* images, uint32_t count);
void gfx_object_free_images(uint32_t baseImageId, uint32_t count);
void gfx_object_check_all_images_freed();
size_t ImageListGetUsedCount();
size_t ImageListGetMaximum();
const std::list<ImageList>& GetAvailableAllocationRanges();
