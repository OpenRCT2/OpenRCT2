/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

struct ScenerySelection
{
    uint8_t SceneryType{};
    ObjectEntryIndex EntryIndex = kObjectEntryIndexNull;

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
        return EntryIndex == kObjectEntryIndexNull;
    }
};
