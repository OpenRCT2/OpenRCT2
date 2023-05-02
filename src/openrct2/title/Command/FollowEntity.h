/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "../../core/String.hpp"
#include "../../localisation/Localisation.h"

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
            utf8 SpriteName[USER_STRING_MAX_LENGTH]{};
        } Follow;

        int16_t operator()(int16_t timer);
    };
} // namespace OpenRCT2::Title
