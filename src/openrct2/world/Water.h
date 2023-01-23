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
namespace OpenRCT2
{
    enum
    {
        WATER_FLAGS_ALLOW_DUCKS = (1 << 0)
    };

#pragma pack(push, 1)
    struct WaterObjectEntry
    {
        StringId string_idx;      // 0x00
        uint32_t image_id;        // 0x02
        uint32_t palette_index_1; // 0x06
        uint32_t palette_index_2; // 0x0A
        uint16_t flags;           // 0x0E
    };
    assert_struct_size(WaterObjectEntry, 16);
#pragma pack(pop)
} // namespace OpenRCT2
