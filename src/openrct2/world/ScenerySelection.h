/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../object/ObjectTypes.h"

#include <cstdint>
#include <limits>

constexpr auto WINDOW_SCENERY_TAB_SELECTION_UNDEFINED = std::numeric_limits<uint16_t>::max();

struct ScenerySelection
{
    uint8_t SceneryType{};
    ObjectEntryIndex EntryIndex = OBJECT_ENTRY_INDEX_NULL;

    inline bool operator==(const ScenerySelection& rhs) const
    {
        return SceneryType == rhs.SceneryType && EntryIndex == rhs.EntryIndex;
    }

    inline bool operator!=(const ScenerySelection& rhs) const
    {
        return !(*this == rhs);
    }

    bool IsUndefined() const
    {
        return EntryIndex == OBJECT_ENTRY_INDEX_NULL;
    }
};
