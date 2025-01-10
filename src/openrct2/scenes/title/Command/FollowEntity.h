/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../Identifiers.h"
#include "../../../core/StringTypes.h"
#include "../../../localisation/Formatting.h"

#include <cstdint>

namespace OpenRCT2::Title
{
    struct FollowEntityCommand
    {
        static constexpr const char* Name = "Follow Entity Command";
        static constexpr const char* ScriptingName = "follow";

        struct
        {
            EntityId SpriteIndex{ EntityId::GetNull() };
            utf8 SpriteName[kUserStringMaxLength]{};
        } Follow;

        int16_t operator()(int16_t timer);
    };
} // namespace OpenRCT2::Title
