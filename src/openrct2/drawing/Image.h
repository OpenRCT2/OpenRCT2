/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstddef>
#include <cstdint>
#include <list>

struct rct_g1_element;

struct ImageList
{
    uint32_t BaseId;
    uint32_t Count;
};

uint32_t gfx_object_allocate_images(const rct_g1_element* images, uint32_t count);
void gfx_object_free_images(uint32_t baseImageId, uint32_t count);
void gfx_object_check_all_images_freed();
size_t ImageListGetUsedCount();
size_t ImageListGetMaximum();
const std::list<ImageList>& GetAvailableAllocationRanges();
