/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../object/Object.h"

#include <cstdint>
#include <limits>

constexpr auto WINDOW_SCENERY_TAB_SELECTION_UNDEFINED = std::numeric_limits<uint16_t>::max();

struct ScenerySelection
{
    uint8_t SceneryType;
    ObjectEntryIndex EntryIndex;

    inline bool operator==(const ScenerySelection& rhs)
    {
        return SceneryType == rhs.SceneryType && EntryIndex == rhs.EntryIndex;
    }

    bool IsUndefined() const
    {
        return EntryIndex == WINDOW_SCENERY_TAB_SELECTION_UNDEFINED;
    }

    void SetUndefined()
    {
        EntryIndex = WINDOW_SCENERY_TAB_SELECTION_UNDEFINED;
    }

    static ScenerySelection CreateUndefined()
    {
        return ScenerySelection{ 0, WINDOW_SCENERY_TAB_SELECTION_UNDEFINED };
    }
};
