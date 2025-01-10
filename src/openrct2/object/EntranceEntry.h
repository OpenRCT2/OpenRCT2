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

struct EntranceEntry
{
    StringId string_idx;    // 0x00
    uint32_t image_id;      // 0x02
    uint8_t scrolling_mode; // 0x06
    uint8_t text_height;    // 0x07
};
