/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../localisation/StringIdType.h"
#include "ObjectTypes.h"
#include "../object/PathAdditionFlags.h"

enum class CursorID : uint8_t;

namespace OpenRCT2
{
    enum class PathAdditionDrawType : uint8_t
    {
        light,
        bin,
        bench,
        jumpingFountain,
    };

    struct PathAdditionEntry
    {
        static constexpr auto kObjectType = ObjectType::pathAdditions;

        StringId name;
        uint32_t image;
        PathAdditionFlags flags;
        PathAdditionDrawType draw_type;
        CursorID tool_id;
        money64 price;
        ObjectEntryIndex scenery_tab_id;
    };
} // namespace OpenRCT2
