/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/StringIdType.h"
#include "ObjectTypes.h"

enum
{
    WATER_FLAGS_ALLOW_DUCKS = (1 << 0)
};

struct WaterObjectEntry
{
    static constexpr auto kObjectType = ObjectType::Water;

    StringId string_idx;      // 0x00
    uint32_t image_id;        // 0x02
    uint32_t palette_index_1; // 0x06
    uint32_t palette_index_2; // 0x0A
    uint16_t flags;           // 0x0E
};
