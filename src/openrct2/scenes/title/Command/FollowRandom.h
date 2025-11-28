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
#include "../../../entity/EntityBase.h"
#include "../../../localisation/Formatting.h"

#include <cstdint>

namespace OpenRCT2::Title
{
    struct FollowRandomCommand
    {
        static constexpr const char* Name = "Follow Random Command";
        static constexpr const char* ScriptingName = "random";

        struct
        {
            EntityType Type{ EntityType::null };
            bool ScrollToLocation{ false };
        } Follow;

        int16_t operator()(int16_t timer);
    };
} // namespace OpenRCT2::Title
