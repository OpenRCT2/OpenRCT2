/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Image.h"

#include "../OpenRCT2.h"
#include "../core/Console.hpp"
#include "../core/Guard.hpp"
#include "../sprites.h"
#include "Drawing.h"

#include <algorithm>
#include <list>

constexpr uint32_t BASE_IMAGE_ID = SPR_IMAGE_LIST_BEGIN;
constexpr uint32_t MAX_IMAGES = SPR_IMAGE_LIST_END - BASE_IMAGE_ID;
constexpr uint32_t INVALID_IMAGE_ID = UINT32_MAX;

static bool _initialised = false;
static std::list<ImageList> _freeLists;
static uint32_t _allocatedImageCount;

#ifdef DEBUG_LEVEL_1
static std::list<ImageList> _allocatedLists;

// MSVC's compiler doesn't support the [[maybe_unused]] attribute for unused static functions. Until this has been resolved, we
// need to explicitly tell the compiler to temporarily disable the warning.
// See discussion at https://github.com/OpenRCT2/OpenRCT2/pull/7617
#    pragma warning(push)
#    pragma warning(disable : 4505) // unreferenced local function has been removed

[[maybe_unused]] static bool AllocatedListContains(uint32_t baseImageId, uint32_t count)
{
    bool contains = std::any_of(
        _allocatedLists.begin(), _allocatedLists.end(), [baseImageId, count](const ImageList& imageList) -> bool {
            return imageList.BaseId == baseImageId && imageList.Count == count;
        });
    return contains;
}

#    pragma warning(pop)

static bool AllocatedListRemove(uint32_t baseImageId, uint32_t count)
{
    auto foundItem = std::find_if(
        _allocatedLists.begin(), _allocatedLists.end(), [baseImageId, count](const ImageList& imageList) -> bool {
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

static uint32_t GetNumFreeImagesRemaining()
{
    return MAX_IMAGES - _allocatedImageCount;
}

static void InitialiseImageList()
{
    Guard::Assert(!_initialised, GUARD_LINE);

    _freeLists.clear();
    _freeLists.push_back({ BASE_IMAGE_ID, MAX_IMAGES });
#ifdef DEBUG_LEVEL_1
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
    _freeLists.sort([](const ImageList& a, const ImageList& b) -> bool { return a.BaseId < b.BaseId; });
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

static uint32_t TryAllocateImageList(uint32_t count)
{
    for (auto it = _freeLists.begin(); it != _freeLists.end(); it++)
    {
        ImageList imageList = *it;
        if (imageList.Count >= count)
        {
            _freeLists.erase(it);
            if (imageList.Count > count)
            {
                ImageList remainder = { imageList.BaseId + count, imageList.Count - count };
                _freeLists.push_back(remainder);
            }

#ifdef DEBUG_LEVEL_1
            _allocatedLists.push_back({ imageList.BaseId, count });
#endif
            _allocatedImageCount += count;
            return imageList.BaseId;
        }
    }
    return INVALID_IMAGE_ID;
}

static uint32_t AllocateImageList(uint32_t count)
{
    Guard::Assert(count != 0, GUARD_LINE);

    if (!_initialised)
    {
        InitialiseImageList();
    }

    uint32_t baseImageId = INVALID_IMAGE_ID;
    uint32_t freeImagesRemaining = GetNumFreeImagesRemaining();
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

static void FreeImageList(uint32_t baseImageId, uint32_t count)
{
    Guard::Assert(_initialised, GUARD_LINE);
    Guard::Assert(baseImageId >= BASE_IMAGE_ID, GUARD_LINE);

#ifdef DEBUG_LEVEL_1
    if (!AllocatedListRemove(baseImageId, count))
    {
        LOG_ERROR("Cannot unload %u items from offset %u", count, baseImageId);
    }
#endif
    _allocatedImageCount -= count;

    for (auto it = _freeLists.begin(); it != _freeLists.end(); it++)
    {
        if (it->BaseId + it->Count == baseImageId)
        {
            it->Count += count;
            return;
        }

        if (baseImageId + count == it->BaseId)
        {
            it->BaseId = baseImageId;
            it->Count += count;
            return;
        }
    }

    _freeLists.push_back({ baseImageId, count });
}

uint32_t GfxObjectAllocateImages(const G1Element* images, uint32_t count)
{
    if (count == 0 || gOpenRCT2NoGraphics)
    {
        return INVALID_IMAGE_ID;
    }

    uint32_t baseImageId = AllocateImageList(count);
    if (baseImageId == INVALID_IMAGE_ID)
    {
        LOG_ERROR("Reached maximum image limit.");
        return INVALID_IMAGE_ID;
    }

    uint32_t imageId = baseImageId;
    for (uint32_t i = 0; i < count; i++)
    {
        GfxSetG1Element(imageId, &images[i]);
        DrawingEngineInvalidateImage(imageId);
        imageId++;
    }

    return baseImageId;
}

void GfxObjectFreeImages(uint32_t baseImageId, uint32_t count)
{
    if (baseImageId != 0 && baseImageId != INVALID_IMAGE_ID)
    {
        // Zero the G1 elements so we don't have invalid pointers
        // and data lying about
        for (uint32_t i = 0; i < count; i++)
        {
            uint32_t imageId = baseImageId + i;
            G1Element g1 = {};
            GfxSetG1Element(imageId, &g1);
            DrawingEngineInvalidateImage(imageId);
        }

        FreeImageList(baseImageId, count);
    }
}

void GfxObjectCheckAllImagesFreed()
{
    if (_allocatedImageCount != 0)
    {
#ifdef DEBUG_LEVEL_1
        Guard::Assert(_allocatedImageCount == 0, "%u images were not freed", _allocatedImageCount);
#else
        Console::Error::WriteLine("%u images were not freed", _allocatedImageCount);
#endif
    }
}

size_t ImageListGetUsedCount()
{
    return _allocatedImageCount;
}

size_t ImageListGetMaximum()
{
    return MAX_IMAGES;
}

const std::list<ImageList>& GetAvailableAllocationRanges()
{
    return _freeLists;
}
