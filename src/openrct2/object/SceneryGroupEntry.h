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
#include "../world/ScenerySelection.h"
#include "ObjectTypes.h"

#include <vector>

struct SceneryGroupEntry
{
    static constexpr auto kObjectType = ObjectType::SceneryGroup;

    StringId name;
    uint32_t image;
    std::vector<ScenerySelection> SceneryEntries;
    uint8_t priority;
    uint32_t entertainer_costumes;
};
