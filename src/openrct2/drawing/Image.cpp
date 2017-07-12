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

#include <algorithm>
#include <list>
#include "../core/Console.hpp"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../OpenRCT2.h"

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
static uint32               _allocatedImageCount;

#ifdef DEBUG
static std::list<ImageList> _allocatedLists;

#pragma warning(push)
#pragma warning(disable : 4505)

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

#pragma warning(pop)

static bool AllocatedListRemove(uint32 baseImageId, uint32 count)
{
    auto foundItem = std::find_if(
        _allocatedLists.begin(),
        _allocatedLists.end(),
        [baseImageId, count](const ImageList &imageList) -> bool
        {
            return imageList.BaseId == baseImageId && imageList.Count == count;
        });
    if (foundItem != _allocatedLists.end())
    {
        _allocatedLists.erase(foundItem);
        return true;
    }
    return false;
}
#endif

static uint32 GetNumFreeImagesRemaining()
{
    return MAX_IMAGES - _allocatedImageCount;
}

static void InitialiseImageList()
{
    Guard::Assert(!_initialised, GUARD_LINE);

    _freeLists.clear();
    _freeLists.push_back({ BASE_IMAGE_ID, MAX_IMAGES });
#ifdef DEBUG
    _allocatedLists.clear();
#endif
    _allocatedImageCount = 0;
    _initialised = true;
}

/**
 * Merges all the free lists into one, a process of defragmentation.
 */
static void MergeFreeLists()
{
    _freeLists.sort(
        [](const ImageList &a, const ImageList &b) -> bool
        {
            return a.BaseId < b.BaseId;
        });
    for (auto it = _freeLists.begin(); it != _freeLists.end(); it++)
    {
        bool mergeHappened;
        do
        {
            mergeHappened = false;
            auto nextIt = std::next(it);
            if (nextIt != _freeLists.end())
            {
                if (it->BaseId + it->Count == nextIt->BaseId)
                {
                    // Merge next list into this list
                    it->Count += nextIt->Count;
                    _freeLists.erase(nextIt);
                    mergeHappened = true;
                }
            }
        } while (mergeHappened);
    }
}

static uint32 TryAllocateImageList(uint32 count)
{
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
            _allocatedImageCount += count;
            return imageList.BaseId;
        }
    }
    return INVALID_IMAGE_ID;
}

static uint32 AllocateImageList(uint32 count)
{
    Guard::Assert(count != 0, GUARD_LINE);

    if (!_initialised)
    {
        InitialiseImageList();
    }

    uint32 baseImageId = INVALID_IMAGE_ID;
    uint32 freeImagesRemaining = GetNumFreeImagesRemaining();
    if (freeImagesRemaining >= count)
    {
        baseImageId = TryAllocateImageList(count);
        if (baseImageId == INVALID_IMAGE_ID)
        {
            // Defragment and try again
            MergeFreeLists();
            baseImageId = TryAllocateImageList(count);
        }
    }
    return baseImageId;
}

static void FreeImageList(uint32 baseImageId, uint32 count)
{
    Guard::Assert(_initialised, GUARD_LINE);
    Guard::Assert(baseImageId >= BASE_IMAGE_ID, GUARD_LINE);

#ifdef DEBUG
    bool contains = AllocatedListRemove(baseImageId, count);
    Guard::Assert(contains, GUARD_LINE);
#endif
    _allocatedImageCount -= count;

    for (auto it = _freeLists.begin(); it != _freeLists.end(); it++)
    {
        if (it->BaseId + it->Count == baseImageId)
        {
            it->Count += count;
            return;
        }
        else if (baseImageId + count == it->BaseId)
        {
            it->BaseId = baseImageId;
            it->Count += count;
            return;
        }
    }

    _freeLists.push_back({ baseImageId, count });
}

extern "C"
{
    uint32 gfx_object_allocate_images(const rct_g1_element * images, uint32 count)
    {
        if (count == 0 || gOpenRCT2NoGraphics)
        {
            return INVALID_IMAGE_ID;
        }

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
        if (baseImageId != 0 && baseImageId != INVALID_IMAGE_ID)
        {
            // Zero the G1 elements so we don't have invalid pointers
            // and data lying about
            for (uint32 i = 0; i < count; i++)
            {
                uint32 imageId = baseImageId + i;
                g1Elements[imageId] = { 0 };
                drawing_engine_invalidate_image(imageId);
            }

            FreeImageList(baseImageId, count);
        }
    }

    void gfx_object_check_all_images_freed()
    {
        if (_allocatedImageCount != 0)
        {
#ifdef DEBUG
            Guard::Assert(_allocatedImageCount == 0, "%u images were not freed", _allocatedImageCount);
#else
            Console::Error::WriteLine("%u images were not freed", _allocatedImageCount);
#endif
        }
    }
}
