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
#include "ObjectTypes.h"

enum class RailingEntrySupportType : uint8_t;

enum
{
    FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR = (1 << 2),
    FOOTPATH_ENTRY_FLAG_IS_QUEUE = (1 << 3),
    FOOTPATH_ENTRY_FLAG_NO_SLOPE_RAILINGS = (1 << 4),
};

struct FootpathEntry
{
    StringId string_idx;                  // 0x00
    uint32_t image;                       // 0x02
    uint32_t bridge_image;                // 0x06
    RailingEntrySupportType support_type; // 0x0A
    uint8_t flags;                        // 0x0B
    uint8_t scrolling_mode;               // 0x0C

    constexpr uint32_t GetQueueImage() const
    {
        return image + 51;
    }
    constexpr uint32_t GetPreviewImage() const
    {
        return image + 71;
    }
    constexpr uint32_t GetQueuePreviewImage() const
    {
        // Editor-only paths usually lack queue images. In this case, use the main path image.
        if (flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR)
        {
            return GetPreviewImage();
        }

        return image + 72;
    }
    constexpr uint32_t GetRailingsImage() const
    {
        return image + 73;
    }
};
