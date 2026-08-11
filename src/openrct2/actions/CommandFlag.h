/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"

#include <cstdint>

namespace OpenRCT2::GameActions
{
    enum class CommandFlag : uint8_t
    {
        /**
         * If this flag is set, the command is applied, otherwise only the cost is retrieved.
         */
        apply,
        /**
         * Command was issued from replay manager.
         */
        replay,
        allowDuringPaused = 3,
        /**
         * Game command is not networked
         */
        noSpend = 5,
        /**
         * Game command is not networked
         */
        ghost = 6,
        trackDesign = 7,
        /**
         * Game command is coming from network
         */
        networked = 31,
    };
    using CommandFlags = FlagHolder<uint32_t, CommandFlag>;
} // namespace OpenRCT2::GameActions
