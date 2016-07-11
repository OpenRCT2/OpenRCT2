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

#include <algorithm>
#include <list>
#include "../core/Console.hpp"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"

extern "C"
{
    #include "drawing.h"
}

constexpr uint32 BASE_IMAGE_ID = 29294;
constexpr uint32 MAX_IMAGES = 262144;
constexpr uint32 INVALID_IMAGE_ID = UINT32_MAX;

struct ImageList
{
    uint32 BaseId;
    uint32 Count;
};

static bool                 _initialised = false;
static std::list<ImageList> _freeLists;

#ifdef DEBUG
static std::list<ImageList> _allocatedLists;

static bool AllocatedListContains(uint32 baseImageId, uint32 count)
{
    bool contains = std::any_of(
        _allocatedLists.begin(),
        _allocatedLists.end(),
        [baseImageId, count](const ImageList &imageList) -> bool
        {
            return imageList.BaseId == baseImageId && imageList.Count == count;
        });
    return contains;
}
#endif

static uint32 AllocateImageList(uint32 count)
{
    Guard::Assert(count != 0);

    if (!_initialised)
    {
        _initialised = true;
        _freeLists.clear();
        _freeLists.push_back({ BASE_IMAGE_ID, MAX_IMAGES });
#ifdef DEBUG
        _allocatedLists.clear();
#endif
    }

    for (auto it = _freeLists.begin(); it != _freeLists.end(); it++)
    {
        ImageList imageList = *it;
        if (imageList.Count >= count)
        {
            _freeLists.erase(it);
            if (imageList.Count > count)
            {
                ImageList remainder = { imageList.BaseId + count,
                                        imageList.Count - count };
                _freeLists.push_back(remainder);
            }

#ifdef DEBUG
            _allocatedLists.push_back({ imageList.BaseId, count });
#endif
            return imageList.BaseId;
        }
    }
    return INVALID_IMAGE_ID;
}

static void FreeImageList(uint32 baseImageId, uint32 count)
{
    Guard::Assert(_initialised);
    Guard::Assert(baseImageId >= BASE_IMAGE_ID);

#ifdef DEBUG
    bool contains = AllocatedListContains(baseImageId, count);
    Guard::Assert(contains);
#endif

    // TODO validate that this was an allocated list
    _freeLists.push_back({ baseImageId, count });
}

extern "C"
{
    uint32 gfx_object_allocate_images(const rct_g1_element * images, uint32 count)
    {
        uint32 baseImageId = AllocateImageList(count);
        if (baseImageId == INVALID_IMAGE_ID)
        {
            log_error("Reached maximum image limit.");
            return INVALID_IMAGE_ID;
        }

        uint32 imageId = baseImageId;
        for (uint32 i = 0; i < count; i++)
        {
            g1Elements[imageId] = images[i];
            drawing_engine_invalidate_image(imageId);
            imageId++;
        }

        return baseImageId;
    }

    void gfx_object_free_images(uint32 baseImageId, uint32 count)
    {
        if (baseImageId != 0)
        {
            FreeImageList(baseImageId, count);
        }
    }
}
