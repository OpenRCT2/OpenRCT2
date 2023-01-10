/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../interface/Cursors.h"

using ObjectEntryIndex = uint16_t;

struct SmallSceneryEntry
{
    StringId name;
    uint32_t image;
    uint32_t flags;
    uint8_t height;
    CursorID tool_id;
    money32 price;
    money32 removal_price;
    uint8_t* frame_offsets;
    uint16_t animation_delay;
    uint16_t animation_mask;
    uint16_t num_frames;
    ObjectEntryIndex scenery_tab_id;

    constexpr bool HasFlag(const uint32_t _flags) const
    {
        return (flags & _flags) != 0;
    }
};
